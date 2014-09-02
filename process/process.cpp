///////////////////////////////////////////////////////////////////////////////////////////////////
#include "process.h"
#include "errno_error.h"

#include <memory>
#include <cstdlib>
#include <ctime>

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace app
{

///////////////////////////////////////////////////////////////////////////////////////////////////
void process::_M_process(std::function<int()> func)
{
    _M_id= fork();
    if(_M_id == -1) throw errno_error();

    if(_M_id == 0)
    {
        int code= func();
        exit(code);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool process::running(bool group)
{
    int code;
    id x= waitpid(group? -_M_id: _M_id, &code, WNOHANG);
    if(x == -1) throw errno_error();

    if(x == 0)
        return false;
    else if(x == _M_id)
    {
        if(WIFEXITED(code))
            _M_code= WEXITSTATUS(code);
        else
        {
            _M_code= -1;
            if(WIFSIGNALED(code)) _M_signal= static_cast<app::signal>(WTERMSIG(code));
        }
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool process::signal(app::signal x, bool group)
{
    int code= ::kill(group? -_M_id: _M_id, int(x));
    if(code == -1)
    {
        if(std::errc(errno) == std::errc::no_such_process)
            return false;
        else throw errno_error();
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace this_process
{

///////////////////////////////////////////////////////////////////////////////////////////////////
process::id get_id() noexcept
{
    return getpid();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
process::id parent_id() noexcept
{
    return getppid();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
struct aren_deleter
{
    void operator()(char* args[])
    {
        for(char* arg= args[0]; arg; ++arg) free(arg);
        delete[] args;
    }
};

typedef std::unique_ptr<char*[], aren_deleter> aren_ptr;

///////////////////////////////////////////////////////////////////////////////////////////////////
static aren_ptr create_arg(const std::string& path, const arguments& args)
{
    aren_ptr value(new char*[args.size()+2]);

    int ri=0;
    value[ri++]= strdup(path.data());

    for(const std::string& x: args) value[ri++]= strdup(x.data());
    value[ri]= nullptr;

    return value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
static aren_ptr create_env(const app::environment& env)
{
    aren_ptr value(new char*[env.size()+1]);

    int ri=0;
    for(auto& x: env) value[ri++]= strdup((x.first+ "="+ x.second).data());
    value[ri]= nullptr;

    return value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int replace(const std::string& path, const arguments& args)
{
    aren_ptr arg= create_arg(path, args);

    if(execv(arg[0], arg.get())) throw errno_error();
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int replace_e(const app::environment& e, const std::string& path, const arguments& args)
{
    aren_ptr env= create_env(e);
    aren_ptr arg= create_arg(path, args);

    if(execve(arg[0], arg.get(), env.get())) throw errno_error();
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int execute(const std::string& command, app::signal* signal)
{
    int code= system(command.data());
    switch(code)
    {
    case  -1: throw execute_error("app::execute failed");
    case 127: throw execute_error("Could not execute shell in the child process");

    default:
        if(WIFEXITED(code)) return WEXITSTATUS(code);

        if(WIFSIGNALED(code) && signal)
            *signal= static_cast<app::signal>(WTERMSIG(code));
        return -1;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void _M_sleep_for(std::chrono::seconds s, std::chrono::nanoseconds ns)
{
    timespec ts=
    {
        static_cast<std::time_t>(s.count()),
        static_cast<long>(ns.count())
    };

    nanosleep(&ts, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}
