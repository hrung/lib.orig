///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "process.h"
#include "except.h"

#include <memory>
#include <cstdlib>
#include <unistd.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace app
{

///////////////////////////////////////////////////////////////////////////////////////////////////
bool process(const std::string& command, int* return_val)
{
    FUNCTION_CONTEXT(ctx);

    int x;
    if(!return_val) return_val= &x;

    *return_val= std::system(command.data());
    if( *return_val == -1 ) throw runtime_error("system() call failed");

    else if(WIFEXITED(*return_val))
        return !WEXITSTATUS(*return_val);

    else return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
pid execute(const std::string& name, const arguments& args)
{
    FUNCTION_CONTEXT(ctx);
    const char* _M_name= name.data();

    std::unique_ptr<const char*> buffer( new const char* [args.size()+2] );
    const char** _M_arg= buffer.get();

    _M_arg[0]= _M_name;

    int rn=1;
    for(auto ri= args.begin(); ri!=args.end(); ++ri, ++rn)
        _M_arg[rn]= ri->data();
    _M_arg[rn]= nullptr;

    pid_t pid= fork();
    switch(pid)
    {
    case -1:
        throw system_error();
        break;

    case 0: // child code
        execvp(_M_name, const_cast<char**>(_M_arg));
        exit(EXIT_FAILURE); // if exec returned, we have a problem...
        break;
    }
    return pid;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}
