///////////////////////////////////////////////////////////////////////////////////////////////////
#include "x11/server.h"

#include <algorithm>
#include <utility>
#include <chrono>
#include <random>
#include <stdexcept>

#include <X11/Xlib.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace app
{

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace x11
{

///////////////////////////////////////////////////////////////////////////////////////////////////
cookie::cookie()
{
    using namespace std::chrono;

    std::default_random_engine dre(system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<char> uni;

    for(size_t ri=0; ri < sizeof(_M_value); ++ri) _M_value[ri]= uni(dre);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string cookie::value() const noexcept
{
    std::string value;
    for(size_t ri=0; ri < sizeof(_M_value); ++ri)
    {
        char lo= _M_value[ri] & 0xf, hi= (_M_value[ri] >> 4) & 0xf;
        value+= hi+ (hi < 10? '0': 'a' - 10);
        value+= lo+ (lo < 10? '0': 'a' - 10);
    }
    return value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
const std::string server::default_name= ":0";

const std::string xorg_path= "/usr/bin/X";
const std::string xauth_path= "/usr/bin/xauth";

///////////////////////////////////////////////////////////////////////////////////////////////////
server::server(const std::string& name, const std::string& server_auth, const arguments& args):
    _M_name(name)
{
    set_cookie(server_auth);
    this_environ::set("XAUTHORITY", server_auth);

    arguments xorg_args;
    xorg_args.push_back(name);
    xorg_args.push_back(args);
    xorg_args.push_back({ "-auth", server_auth });

    process proc(process::group, this_process::replace, xorg_path, xorg_args);
    std::swap(proc, _M_process);

    for(int ri=0; ri<10; ++ri)
    {
        this_process::sleep_for(std::chrono::seconds(1));

        if(!_M_process.running()) throw std::runtime_error("X server failed to start");

        _M_display= XOpenDisplay(_M_name.data());
        if(_M_display) break;
    }

    if(!_M_display) throw std::runtime_error("X server failed to initialize");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
server::~server()
{
    if(_M_process.running())
    {
        if(_M_display)
        {
            XCloseDisplay(_M_display);
            _M_display= nullptr;
        }

        _M_process.signal(signal::hangup);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void server::set_cookie(const std::string& path)
{
    process xauth(redir::cin, this_process::replace, xauth_path, arguments { "-q", "-f", path });

    xauth.cin << "remove " << name() << std::endl;
    xauth.cin << "add " << name() << " . " << _M_cookie.value() << std::endl;
    xauth.cin << "exit" << std::endl;
    xauth.join();

    if(xauth.exit_code().code()) throw std::runtime_error("Could not set server auth");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}
