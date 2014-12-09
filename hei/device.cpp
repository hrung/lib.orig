///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "device.h"
#include "hei_error.h"

#include <cstring>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace hei
{

///////////////////////////////////////////////////////////////////////////////////////////////////
constexpr unsigned max_number= 999999999;

constexpr unsigned PLC_QUERY_TIMEOUT= 2000; // in milliseconds
constexpr unsigned PLC_COMMAND_TIMEOUT= 300;

constexpr unsigned PLC_OPEN_TIMEOUT= 100;
constexpr unsigned PLC_OPEN_RETRIES= 3;

///////////////////////////////////////////////////////////////////////////////////////////////////
struct interface
{
    static int count;

    static void open()
    {
        HEIOpen(HEIAPIVERSION);
        ++count;
    }

    static void close()
    {
        if(count)
        {
            HEIClose();
            --count;
        }
    }
};

int interface::count=0;

///////////////////////////////////////////////////////////////////////////////////////////////////
device::device(transport tran, protocol proto, unsigned number, const std::string& name)
{
    // initialize interface
    interface::open();

    _M_tran= new HEITransport;
    std::memset(_M_tran, 0, sizeof(HEITransport));

    _M_tran->Transport= static_cast<int>(tran);
    _M_tran->Protocol= static_cast<int>(proto);

    int err= HEIOpenTransport(_M_tran, HEIAPIVERSION, NULL);
    if(err) throw hei_error(err);

    // locate device
    WORD count=1;

    _M_dev= new HEIDevice;
    std::memset(_M_dev, 0, sizeof(HEIDevice));

    HEISetQueryTimeout(PLC_QUERY_TIMEOUT);

    if(number > max_number)
        throw std::invalid_argument("device::device(): number must be <= " + std::to_string(max_number));

    if(number == 0 && name.empty())
        throw std::invalid_argument("device::device(): either name or number must be specified");
    else if(name.empty())
    {
        DWORD num= number;
        err= HEIQueryDeviceData(_M_tran, _M_dev, &count,
                                HEIAPIVERSION,
                                DT_NODE_NUMBER,
        (BYTE*)(&num), sizeof(num));
    }
    else err= HEIQueryDeviceData(_M_tran, _M_dev, &count,
                                 HEIAPIVERSION,
                                 DT_NODE_NAME,
         (BYTE*)(name.data()), name.size());
    if(err) throw hei_error(err);

    if(count == 0)
        throw hei_error(errc::timeout,
    "device::device(): device #" + std::to_string(number) + " '" + name + "' not found");

    // open device
    err= HEIOpenDevice(_M_tran, _M_dev, HEIAPIVERSION, PLC_OPEN_TIMEOUT, PLC_OPEN_RETRIES, FALSE);
    if(err) throw hei_error(err);
    _M_dev->Timeout= PLC_COMMAND_TIMEOUT;

    // get device data
    DeviceDef dd;
    std::memset(&dd, 0, sizeof(dd));

    err= HEIReadDeviceDef(_M_dev, (BYTE*)(&dd), sizeof(dd));
    if(err) throw hei_error(err);

    _M_family= hei::family(dd.PLCFamily);
    _M_type= hei::module_type(dd.ModuleType);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void device::close() noexcept
{
    if(_M_dev)
    {
        // close device
        HEICloseDevice(_M_dev);
        delete _M_dev;
        _M_dev= nullptr;

        // close transport
        HEICloseTransport(_M_tran);
        delete _M_tran;
        _M_tran= nullptr;

        // shutdown interface
        interface::close();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void device::read_data(data_type type, unsigned address, unsigned count, void* buf)
{
    int err= HEICCMRequest(_M_dev, FALSE, static_cast<int>(type), address, count, static_cast<BYTE*>(buf));
    if(err) throw hei_error(err);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void device::write_data(data_type type, unsigned address, unsigned count, void* buf)
{
    int err= HEICCMRequest(_M_dev, TRUE, static_cast<int>(type), address, count, static_cast<BYTE*>(buf));
    if(err) throw hei_error(err);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool device::read_input(unsigned offset, unsigned input)
{
    unsigned address= offset+ (input-1)/8;
    BYTE bit_mask= 1 << ((input-1)%8);
    BYTE value;

    read_data(data_type::input, address, sizeof(value), &value);
    return value & bit_mask;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}
