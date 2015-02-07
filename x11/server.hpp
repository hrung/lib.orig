///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef SERVER_HPP
#define SERVER_HPP

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "process/process.hpp"

#include <string>

///////////////////////////////////////////////////////////////////////////////////////////////////
struct _XDisplay;

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace x11
{

///////////////////////////////////////////////////////////////////////////////////////////////////
typedef _XDisplay* display;

///////////////////////////////////////////////////////////////////////////////////////////////////
class cookie
{
public:
    cookie();
    std::string value() const noexcept;

private:
    char _M_value[16];
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class server
{
public:
    static const std::string default_name;

public:
    server() = default;
    server(const server&) = delete;

    server(server&& x) noexcept { swap(x); }

    server(const std::string& name, const std::string& server_auth, const app::arguments& args = {});
    explicit server(const std::string& server_auth, const app::arguments& args = {}):
        server(default_name, server_auth, args)
    { }
    ~server();

    void close();

    server& operator=(const server&) = delete;
    server& operator=(server&& x) noexcept
    {
        swap(x);
        return (*this);
    }

    void swap(server& x) noexcept
    {
        std::swap(_M_name, x._M_name);
        std::swap(_M_cookie, x._M_cookie);

        std::swap(_M_process, x._M_process);
        std::swap(_M_display, x._M_display);
    }

    ////////////////////
    const std::string& name() const noexcept { return _M_name; }

    bool running() { return _M_process.running(); }

    x11::display display() const noexcept { return _M_display; }

    void set_cookie(const std::string& path);

private:
    std::string _M_name;
    x11::cookie _M_cookie;

    app::process _M_process;
    x11::display _M_display = nullptr;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // SERVER_HPP
