///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "process.h"
#include "except.h"

#include <vector>
#include <memory>
#include <cstdlib>

#include <unistd.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace app
{

///////////////////////////////////////////////////////////////////////////////////////////////////
bool process(const std::string& command, int* return_val)
{
    int x;
    if(!return_val) return_val= &x;

    *return_val= std::system(command.data());
    if( *return_val == -1 ) throw std::runtime_error("system() call failed");

    else if(WIFEXITED(*return_val))
        return !WEXITSTATUS(*return_val);

    else return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
pid execute(const std::string& name, const arguments& args)
{
    arguments c;
    c.push_back(name);
    c.insert(c.end(), args.begin(), args.end());

    std::vector<char*> _M_arg;
    std::vector<std::unique_ptr<char[]>> _M_arg_data;

    for(const std::string& arg: c)
    {
        int length= arg.size();
        _M_arg_data.emplace_back(new char[length+1]);

        arg.copy(_M_arg_data.back().get(), length);
        _M_arg_data.back()[length]=0;

        _M_arg.push_back(_M_arg_data.back().get());
    }
    _M_arg.push_back(nullptr);

    pid_t pid= fork();
    switch(pid)
    {
    case -1:
        throw system_error();
        break;

    case 0: // child code
        execvp(_M_arg[0], _M_arg.data());
        exit(EXIT_FAILURE); // if exec returned, we have a problem...
        break;
    }
    return pid;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}
