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
    device& operator=(const device&) = delete;

    device(device&&) = default;
    device& operator=(device&&) = default;

    virtual ~device() { close(); }

    void open(transport, protocol, unsigned number, const std::string& name= std::string());
    void close();
    bool is_open() const { return _M_open; }

    hei::family family() const { return _M_family; }
    hei::module_type module_type() const { return _M_type; }

    void read_data(data_type, unsigned address, unsigned count, void* buffer);
    void write_data(data_type, unsigned address, unsigned count, void* buffer);

    bool read_input(unsigned offset, unsigned input);

    static int count() { return _M_count; }

private:
    bool _M_open= false;

    HEITransport _M_tran;
    HEIDevice _M_dev;

    hei::family _M_family= family::none;
    hei::module_type _M_type= module_type::none;

    static void lib_open();
    static void lib_close();

    static int _M_count;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif
