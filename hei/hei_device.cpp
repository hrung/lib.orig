///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "hei_device.hpp"
#include "hei_error.hpp"

#include <cstring>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace hei
{

///////////////////////////////////////////////////////////////////////////////////////////////////
constexpr unsigned max_number = 999999999;

constexpr unsigned plc_query_timeout = 2000; // in milliseconds
constexpr unsigned plc_command_timeout = 300;

constexpr unsigned plc_open_timeout = 100;
constexpr unsigned plc_open_retries = 3;

///////////////////////////////////////////////////////////////////////////////////////////////////
struct interface
{
    static int count;

    static void open() noexcept
    {
        HEIOpen(HEIAPIVERSION);
        ++count;
    }

    static void close() noexcept
    {
        if(count)
        {
            HEIClose();
            --count;
        }
    }
};

int interface::count = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
device::device(transport tran, protocol proto, unsigned number, const std::string& name)
{
    // initialize interface
    interface::open();

    _M_tran = new HEITransport;
    std::memset(_M_tran, 0, sizeof(HEITransport));

    _M_tran->Transport = static_cast<int>(tran);
    _M_tran->Protocol = static_cast<int>(proto);

    int code = HEIOpenTransport(_M_tran, HEIAPIVERSION, NULL);
    if(code) throw hei_error(code);

    // locate device
    WORD count = 1;

    _M_dev = new HEIDevice;
    std::memset(_M_dev, 0, sizeof(HEIDevice));

    HEISetQueryTimeout(plc_query_timeout);

    if(number > max_number)
        throw std::invalid_argument("device::device(): number must be <= " + std::to_string(max_number));

    if(number == 0 && name.empty())
        throw std::invalid_argument("device::device(): either name or number must be specified");
    else if(name.empty())
    {
        DWORD num = number;
        code = HEIQueryDeviceData(_M_tran, _M_dev, &count, HEIAPIVERSION, DT_NODE_NUMBER, (BYTE*)(&num), sizeof(num));
    }
    else code = HEIQueryDeviceData(_M_tran, _M_dev, &count, HEIAPIVERSION, DT_NODE_NAME, (BYTE*)(name.data()), name.size());
    if(code) throw hei_error(code);

    if(count == 0)
        throw hei_error(errc::timeout,
    "device::device(): device #" + std::to_string(number) + " '" + name + "' not found");

    // open device
    code = HEIOpenDevice(_M_tran, _M_dev, HEIAPIVERSION, plc_open_timeout, plc_open_retries, FALSE);
    if(code) throw hei_error(code);
    _M_dev->Timeout = plc_command_timeout;

    // get device data
    DeviceDef dd;
    std::memset(&dd, 0, sizeof(dd));

    code = HEIReadDeviceDef(_M_dev, (BYTE*)(&dd), sizeof(dd));
    if(code) throw hei_error(code);

    _M_family = hei::family(dd.PLCFamily);
    _M_type = hei::module_type(dd.ModuleType);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void device::close() noexcept
{
    if(_M_dev)
    {
        // close device
        HEICloseDevice(_M_dev);
        delete _M_dev;
        _M_dev = nullptr;
    }

    if(_M_tran)
    {
        // close transport
        HEICloseTransport(_M_tran);
        delete _M_tran;
        _M_tran = nullptr;
    }

    // shutdown interface
    interface::close();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void device::read_data(data_type type, unsigned address, unsigned count, void* buffer)
{
    int code = HEICCMRequest(_M_dev, FALSE, static_cast<int>(type), address, count, static_cast<BYTE*>(buffer));
    if(code) throw hei_error(code);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void device::write_data(data_type type, unsigned address, unsigned count, void* buffer)
{
    int code = HEICCMRequest(_M_dev, TRUE, static_cast<int>(type), address, count, static_cast<BYTE*>(buffer));
    if(code) throw hei_error(code);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool device::read_input(unsigned offset, unsigned input)
{
    unsigned address = offset + (input - 1) / 8;
    BYTE bit_mask = 1 << ((input - 1) % 8);
    BYTE value;

    read_data(data_type::input, address, sizeof(value), &value);
    return value & bit_mask;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}
