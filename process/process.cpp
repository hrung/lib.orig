///////////////////////////////////////////////////////////////////////////////////////////////////
#include "process.h"
#include "message.h"
#include "stream.h"
#include "except.h"

#include <memory>

#include <unistd.h>
#include <stdlib.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace sys
{

///////////////////////////////////////////////////////////////////////////////////////////////////
bool process(const std::string& command)
{
    show_message( "Launching "+ command );

    int value= system(command.data());
    if(value==-1)
        show_error("Execution failed");
    else if(WIFEXITED(value))
    {
        if(( value= WEXITSTATUS(value) ))
            show_error( std::string("Exited with status ") << value );
        else return true;
    }
    else if(WIFSIGNALED(value))
        show_error( std::string("Exited due to signal ") << WTERMSIG(value) );

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
pid_t execute(const std::string& name, const arguments& args)
{
    const char* _M_name= name.data();

    std::unique_ptr<const char*> buffer( new const char* [args.size()+2] );
    const char** _M_arg= buffer.get();

    _M_arg[0]= _M_name;

    std::string stream;
    stream+= "Launching "+ name;

    int rn=1;
    for(auto ri= args.begin(); ri!=args.end(); ++ri, ++rn)
    {
        _M_arg[rn]= ri->data();
        stream+= ' '+ *ri;
    }
    _M_arg[rn]= nullptr;

    show_message(stream);

    pid_t pid= fork();
    switch(pid)
    {
    case -1:
        throw system_except();
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
