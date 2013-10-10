///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef EXCEPT_H
#define EXCEPT_H

///////////////////////////////////////////////////////////////////////////////////////////////////
#include <system_error>
#include <stdexcept>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace except
{

///////////////////////////////////////////////////////////////////////////////////////////////////
class exception: public std::system_error
{
public:
    exception(const std::string& message, std::error_code code= std::error_code()):
        std::system_error(code),
        _M_message(message)
    { }

    exception(const std::string& file, const std::string& func, int line, const std::string& message, std::error_code code= std::error_code()):
        std::system_error(code),
        _M_message(message),
        _M_file(file), _M_func(func), _M_line(line)
    { }

    std::string message() const { return _M_message; }
    std::string file() const { return _M_file; }
    std::string func() const { return _M_func; }
    int line() const { return _M_line; }

protected:
    std::string _M_message;
    std::string _M_file;
    std::string _M_func;
    int _M_line=0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class critical_exception: public exception
{
public:
    using exception::exception;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class not_exception: public exception
{
public:
    using exception::exception;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class system_exception: public exception
{
public:
    system_exception(const std::string& message= std::string()):
        exception(message, std::error_code(errno, std::generic_category()))
    { }

    system_exception(const std::string& file, const std::string& func, int line, const std::string& message= std::string()):
        exception(file, func, line, message, std::error_code(errno, std::generic_category()))
    { }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class user_exception: public exception
{
public:
    user_exception(const std::string& message, const std::string& user_message):
        exception(message),
        _M_user_message(user_message)
    { }

    user_exception(const std::string& file, const std::string& func, int line, const std::string& message, const std::string& user_message):
        exception(file, func, line, message),
        _M_user_message(user_message)
    { }

    std::string user_message() const { return _M_user_message; }

protected:
    std::string _M_user_message;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
#define except(args...)             except::exception         (__FILE__, __FUNCTION__, __LINE__, args)

#define critical_except(args...)    except::critical_exception(__FILE__, __FUNCTION__, __LINE__, args)
#define not_except(args...)         except::not_exception     (__FILE__, __FUNCTION__, __LINE__, args)

#define system_except(args...)      except::system_exception  (__FILE__, __FUNCTION__, __LINE__, ##args)
#define user_except(args...)        except::user_exception    (__FILE__, __FUNCTION__, __LINE__, args)

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif
