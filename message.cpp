///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2013 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "message.h"
#include "common.h"
#include "stream.h"
#include <iostream>

///////////////////////////////////////////////////////////////////////////////////////////////////
void show_message(const std::string& message, level l)
{
    if(logging) syslog(int(l), "%s", message.data());
    if(verbose) std::cerr << message << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void show_error(const std::string& message)
{
    bool _M_verbose= verbose;
    verbose= true;

    show_message(message, level::error);
    verbose= _M_verbose;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void show_error(const except::exception& e)
{
    std::string stream= "Error: ";

    if(e.message().size())
    {
        stream << e.message() << " ";
        if(e.code().value()) stream << "(" << e.code().message() << ", " << e.code().value() << ") ";
    }
    else if(e.code().value()) stream << e.code().message() << " (" << e.code().value() << ") ";

    if(e.func().size()) stream << "in function " << e.func() << "() at " << e.file() << ", " << e.line();
    show_error(stream);
}
