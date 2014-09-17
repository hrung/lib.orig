///////////////////////////////////////////////////////////////////////////////////////////////////
#include "x11.h"

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
const std::string server::default_name= ":0.0";
const std::string server::default_path= "/usr/bin/X";
const arguments server::default_args= { "-br", "-novtswitch", "-nolisten", "tcp", "-quiet" };

///////////////////////////////////////////////////////////////////////////////////////////////////
server::server(const std::string& auth, const std::string& name, const std::string& path, const arguments& args):
    _M_name(name.size()? name: default_name)
{
    arguments full;
    full.push_back(_M_name);

    if(args.size())
        std::copy(args.begin(), args.end(), std::back_inserter(full));
    else
        std::copy(default_args.begin(), default_args.end(), std::back_inserter(full));

    full.push_back("-auth");
    full.push_back(auth);

    process proc(true, this_process::replace, path.size()? path: default_path, full);
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
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}
