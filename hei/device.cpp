///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2013 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "device.h"
#include "hei_error.h"
#include "stream.h"

#include <string.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace hei
{

///////////////////////////////////////////////////////////////////////////////////////////////////
constexpr unsigned MAX_MODULE_NUM= 999999999;

constexpr unsigned PLC_QUERY_TIMEOUT= 2000; // in milliseconds
constexpr unsigned PLC_COMMAND_TIMEOUT= 300;

constexpr unsigned PLC_OPEN_TIMEOUT= 100;
constexpr unsigned PLC_OPEN_RETRIES= 3;

///////////////////////////////////////////////////////////////////////////////////////////////////
int device::_M_count=0;

///////////////////////////////////////////////////////////////////////////////////////////////////
void device::lib_open()
{
    HEIOpen(HEIAPIVERSION);
    ++_M_count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void device::lib_close()
{
    if(_M_count)
    {
        HEIClose();
        --_M_count;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void device::close()
{
    if(_M_open)
    {
        // close device
        HEICloseDevice(&_M_dev);

        // close transport
        HEICloseTransport(&_M_tran);

        // shutdown interface
        lib_close();

        _M_family= family::none;
        _M_type= module_type::none;
        _M_open= false;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void device::open(transport tran, protocol proto, unsigned number, const std::string& name)
{
    // close device if it's already open
    close();

    // initialize interface
    lib_open();

    // open transport
    if(tran != transport::unix || proto != protocol::ip)
        throw hei_except(errc::user_error, "Only UNIX/IP is supported");

    memset(&_M_tran, 0, sizeof(_M_tran));
    _M_tran.Transport= int(tran);
    _M_tran.Protocol= int(proto);

    int err= HEIOpenTransport(&_M_tran, HEIAPIVERSION, NULL);
    if(err) throw hei_except(err);

    // locate device
    WORD count=1;
    memset(&_M_dev, 0, sizeof(_M_dev));

    HEISetQueryTimeout(PLC_QUERY_TIMEOUT);

    if(number > MAX_MODULE_NUM)
        throw hei_except(errc::user_error, std::string("Device number must be <= ") << MAX_MODULE_NUM);

    if(number == 0 && name.empty())
        throw hei_except(errc::user_error, "Device name or number must be specified");
    else if(name.empty())
    {
        DWORD num= number;
        err= HEIQueryDeviceData(&_M_tran, &_M_dev, &count, HEIAPIVERSION, DT_NODE_NUMBER, (BYTE*)&num, sizeof(num));
    }
    else err= HEIQueryDeviceData(&_M_tran, &_M_dev, &count, HEIAPIVERSION, DT_NODE_NAME, (BYTE*)name.data(), name.size());
    if(err) throw hei_except(err);

    if(count == 0)
        throw hei_except(errc::timeout, std::string("Device [number=") << number << " name=" << name << "] not found");

    // open device
    err= HEIOpenDevice(&_M_tran, &_M_dev, HEIAPIVERSION, PLC_OPEN_TIMEOUT, PLC_OPEN_RETRIES, FALSE);
    if(err) throw hei_except(err);
    _M_dev.Timeout= PLC_COMMAND_TIMEOUT;

    // get device data
    DeviceDef dd;
    memset(&dd, 0, sizeof(dd));

    err= HEIReadDeviceDef(&_M_dev, (BYTE*)&dd, sizeof(dd));
    if(err) throw hei_except(err);

    _M_family= hei::family(dd.PLCFamily);
    _M_type= hei::module_type(dd.ModuleType);
    _M_open= true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void device::read_data(data_type type, unsigned address, unsigned count, void* buf)
{
    if(!_M_open) throw except("Device not open");

    int err= HEICCMRequest(&_M_dev, FALSE, int(type), address, count, (BYTE*)buf);
    if(err) throw hei_except(err);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void device::write_data(data_type type, unsigned address, unsigned count, void* buf)
{
    if(!_M_open) throw except("Device not open");

    int err= HEICCMRequest(&_M_dev, TRUE, int(type), address, count, (BYTE*)buf);
    if(err) throw hei_except(err);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool device::read_input(unsigned offset, unsigned input)
{
    if(!_M_open) throw hei_except(errc::user_error, "Device not open");

    unsigned address= offset+ (input-1)/8;
    BYTE bit_mask= 1 << ((input-1)%8);
    BYTE value;

    read_data(data_type::input, address, sizeof(value), &value);
    return value & bit_mask;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}
