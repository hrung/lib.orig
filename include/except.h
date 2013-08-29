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
    exception(const std::string& message): _M_message(message) { }

    exception(const std::string& file, const std::string& func, int line, const std::string& message):
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

    exception(const std::string& message, std::error_code code):
        std::system_error(code),
        _M_message(message)
    { }

    exception(const std::string& file, const std::string& func, int line, const std::string& message, std::error_code code):
        std::system_error(code),
        _M_message(message),
        _M_file(file), _M_func(func), _M_line(line)
    { }
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
        exception(message, std::error_code(errno, std::system_category()))
    { }

    system_exception(const std::string& file, const std::string& func, int line, const std::string& message= std::string()):
        exception(file, func, line, message, std::error_code(errno, std::system_category()))
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

#define except(...)          except::exception(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

#define critical_except(...) except::critical_exception(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define not_except(...)      except::not_exception(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

#define system_except()      except::system_exception(__FILE__, __FUNCTION__, __LINE__)
#define user_except(...)     except::user_exception(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif
