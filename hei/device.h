///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef DEVICE_H
#define DEVICE_H

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "hei_type.h"
#include <string>

#define HEIUNIX
#include <hei.h>

#undef min
#undef max

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace hei
{

///////////////////////////////////////////////////////////////////////////////////////////////////
class device
{
public:
    device() = default;
    device(const device&) = delete;
    device(device&& x) noexcept { swap(x); }

    device(transport, protocol, unsigned number, const std::string& name= std::string());
    virtual ~device();

    device& operator=(const device&) = delete;
    device& operator=(device&& x) noexcept
    {
        swap(x);
        return (*this);
    }

    void swap(device& x) noexcept
    {
        std::swap(_M_open,   x._M_open);
        std::swap(_M_tran,   x._M_tran);
        std::swap(_M_dev,    x._M_dev);
        std::swap(_M_family, x._M_family);
        std::swap(_M_type,   x._M_type);
    }

    bool open() const noexcept { return _M_open; }

    hei::family family() const noexcept { return _M_family; }
    hei::module_type module_type() const noexcept { return _M_type; }

    void read_data(data_type, unsigned address, unsigned count, void* buffer);
    void write_data(data_type, unsigned address, unsigned count, void* buffer);

    bool read_input(unsigned offset, unsigned input);

private:
    bool _M_open= false;

    HEITransport _M_tran;
    HEIDevice _M_dev;

    hei::family _M_family= family::none;
    hei::module_type _M_type= module_type::none;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif
