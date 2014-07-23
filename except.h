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

#include <stack>
#include <string>

// for applications compiled with -pthread
#ifdef _REENTRANT
#  include <thread>
#  include <mutex>
#  include <map>
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
class backtrace: public std::stack<std::string>
{
public:
    std::string write(const std::string& separator= "\n") const;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class context
{
public:
    context(const std::string& name) { push(name); }
    context(std::string&& name) { push(std::move(name)); }
   ~context() { pop(); }

    static backtrace current();

private:
    void push(const std::string& name);
    void push(std::string&& name);
    void pop();

#ifdef _REENTRANT
    static std::map<std::thread::id, backtrace> _M_current;
    static std::recursive_mutex _M_mutex;
#else
    static backtrace _M_current;
#endif
};

#define FUNCTION_CONTEXT(ctx) context ctx(std::string()+ "Function: "+ __FUNCTION__+ "()")

///////////////////////////////////////////////////////////////////////////////////////////////////
class context_error
{
public:
    context_error(): _M_trace(context::current()) { }
    const backtrace& trace() const { return _M_trace; }

private:
    backtrace _M_trace;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
class exception: public std::exception, public context_error
{
public:
    using std::exception::exception;
    exception(const std::exception& e): std::exception(e) { }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class logic_error: public std::logic_error, public context_error
{
public:
    using std::logic_error::logic_error;
    logic_error(const std::logic_error& e): std::logic_error(e) { }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class domain_error: public std::domain_error, public context_error
{
public:
    using std::domain_error::domain_error;
    domain_error(const std::domain_error& e): std::domain_error(e) { }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class invalid_argument: public std::invalid_argument, public context_error
{
public:
    using std::invalid_argument::invalid_argument;
    invalid_argument(const std::invalid_argument& e): std::invalid_argument(e) { }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class length_error: public std::length_error, public context_error
{
public:
    using std::length_error::length_error;
    length_error(const std::length_error& e): std::length_error(e) { }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class out_of_range: public std::out_of_range, public context_error
{
public:
    using std::out_of_range::out_of_range;
    out_of_range(const std::out_of_range& e): std::out_of_range(e) { }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class runtime_error: public std::runtime_error, public context_error
{
public:
    using std::runtime_error::runtime_error;
    runtime_error(const std::runtime_error& e): std::runtime_error(e) { }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class range_error: public std::range_error, public context_error
{
public:
    using std::range_error::range_error;
    range_error(const std::range_error& e): std::range_error(e) { }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class overflow_error: public std::overflow_error, public context_error
{
public:
    using std::overflow_error::overflow_error;
    overflow_error(const std::overflow_error& e): std::overflow_error(e) { }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class underflow_error: public std::underflow_error, public context_error
{
public:
    using std::underflow_error::underflow_error;
    underflow_error(const std::underflow_error& e): std::underflow_error(e) { }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
class system_error: public std::system_error, public context_error
{
public:
    system_error(): std::system_error(std::error_code(errno, std::generic_category())) { }
    system_error(const std::string& message): std::system_error(std::error_code(errno, std::generic_category()), message) { }

    system_error(std::error_code c): std::system_error(c) { }
    system_error(std::error_code c, const std::string& message): std::system_error(c, message) { }

    system_error(int v, const std::error_category& c): std::system_error(v, c) { }
    system_error(int v, const std::error_category& c, const std::string& message): std::system_error(v, c, message) { }

    system_error(const std::system_error& e): std::system_error(e) { }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif
