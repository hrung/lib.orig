///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ERRNO_ERROR_HPP
#define ERRNO_ERROR_HPP

///////////////////////////////////////////////////////////////////////////////////////////////////
#include <cerrno>
#include <string>
#include <system_error>

///////////////////////////////////////////////////////////////////////////////////////////////////
class errno_error: public std::system_error
{
public:
    errno_error(): std::system_error(std::error_code(errno, std::generic_category())) { }
    errno_error(const std::string& message): std::system_error(std::error_code(errno, std::generic_category()), message) { }

    errno_error(std::error_code c): std::system_error(c) { }
    errno_error(std::error_code c, const std::string& message): std::system_error(c, message) { }

    errno_error(int v, const std::error_category& c): std::system_error(v, c) { }
    errno_error(int v, const std::error_category& c, const std::string& message): std::system_error(v, c, message) { }

    errno_error(const std::system_error& e): std::system_error(e) { }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // ERRNO_ERROR_HPP
