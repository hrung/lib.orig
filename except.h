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

#include <string>

///////////////////////////////////////////////////////////////////////////////////////////////////
using std::exception;
using std::logic_error;
using std::domain_error;
using std::invalid_argument;
using std::length_error;
using std::out_of_range;
using std::runtime_error;
using std::range_error;
using std::overflow_error;
using std::underflow_error;

///////////////////////////////////////////////////////////////////////////////////////////////////
class system_error: public std::system_error
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
