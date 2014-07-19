///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef EXCEPT_H
#define EXCEPT_H

///////////////////////////////////////////////////////////////////////////////////////////////////
#include <system_error>
#include <cerrno>

#include <sstream>
#include <vector>
#include <string>

///////////////////////////////////////////////////////////////////////////////////////////////////
class error: public std::system_error
{
public:
    error(): std::system_error(std::error_code(errno, std::generic_category())) { }
    error(const std::string& message):
        std::system_error(std::error_code(errno, std::generic_category()), message)
    { }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class context;
typedef std::vector<context> context_trace;

class context
{
public:
    context(const std::string& name):
        _M_trace(_M_global)
    { _M_trace.push_back(name); }

    context(std::string&& name):
        _M_trace(_M_global)
    { _M_trace.push_back(std::move(name)); }

    context(const std::string& name, context_trace& x):
        _M_trace(x)
    { _M_trace.push_back(name); }

    context(std::string&& name, context_trace& x):
        _M_trace(x)
    { _M_trace.push_back(std::move(name)); }

    ~context() { _M_trace.pop_back(); }

    static const context_trace& global_trace() { return _M_global; }

private:
    context_trace& _M_trace;

    static context_trace _M_global;
};

std::ostream& operator<<(std::ostream& stream, const context_trace& x)
{
    for(auto& ctx: x) stream << ctx << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif
