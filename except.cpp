///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "except.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef _REENTRANT
std::map<std::thread::id, backtrace> context::_M_current;
std::mutex context::_M_mutex;
#else
backtrace context::_M_current;
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
void context::push(const std::string& name)
{
#ifdef _REENTRANT
    std::lock_guard<std::mutex> lock(_M_mutex);
    _M_current[std::this_thread::get_id()].push(name);
#else
    _M_current.push(name);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void context::push(std::string&& name)
{
#ifdef _REENTRANT
    std::lock_guard<std::mutex> lock(_M_mutex);
    _M_current[std::this_thread::get_id()].push(std::move(name));
#else
    _M_current.push(std::move(name));
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void context::pop()
{
#ifdef _REENTRANT
    std::lock_guard<std::mutex> lock(_M_mutex);
    _M_current[std::this_thread::get_id()].pop();
#else
    _M_current.pop();
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
backtrace context::current()
{
#ifdef _REENTRANT
    std::lock_guard<std::mutex> lock(_M_mutex);
    return _M_current[std::this_thread::get_id()];
#else
    return _M_current;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string backtrace::write(const std::string& separator) const
{
    std::string string;
    if(c.size())
    {
        auto ri= c.crbegin();
        string+= *ri;

        while(++ri != c.crend()) string+= separator+ *ri;
    }
    return string;
}
