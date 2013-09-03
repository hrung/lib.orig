///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2013 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "common.h"
#include "stream.h"

#include <iostream>
#include <memory>

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string prog_name;

bool verbose= false;
bool logging= false;

bool exiting= false;
int  exit_code=0;

///////////////////////////////////////////////////////////////////////////////////////////////////
void show_message(const std::string& what, level l, bool force)
{
    if(logging) syslog(int(l), "%s", what.data());
    if(verbose || force) std::cerr << what << std::endl;
}

void show_message(const except::exception& e, bool force)
{
    stream m("Error: ");

    if(e.message().size())
    {
        m << e.message() << " ";
        if(e.code().value()) m << "(" << e.code().message() << ", " << e.code().value() << ") ";
    }
    else if(e.code().value()) m << e.code().message() << " (" << e.code().value() << ") ";

    if(e.func().size()) m << "in function " << e.func() << "() at " << e.file() << ", " << e.line();
    show_message(m, level::error, force);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool system(const std::string& command)
{
    int val= system(command.data());
    if(val==-1)
        show_message("system() failed", level::error);
    else if(WIFEXITED(val))
    {
        if( (val= WEXITSTATUS(val)) )
            show_message( stream() << "system() exited with status " << val, level::error );
        else return true;
    }
    else if(WIFSIGNALED(val))
        show_message( stream() << "system() exited with signal " << WTERMSIG(val), level::error );

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
pid_t execute(const std::string& name, const arguments& args)
{
    const char* _M_name= name.data();

    std::unique_ptr<const char*> buffer( new const char* [args.size()+2] );
    const char** _M_args= buffer.get();

    _M_args[0]= _M_name;

    int rn=1;
    for(const std::string& arg: args)
    {
        _M_args[rn]= arg.data();
        ++rn;
    }
    _M_args[rn]= nullptr;

    pid_t pid= fork();
    switch(pid)
    {
    case -1:
        throw system_except();
        break;

    case 0: // child code
        execvp(_M_name, const_cast<char* const *>(_M_args));
        exit(EXIT_FAILURE); // if exec returned, we have a problem...
        break;
    }
    return pid;
}
