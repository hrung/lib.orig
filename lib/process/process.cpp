///////////////////////////////////////////////////////////////////////////////////////////////////
#include "process.h"
#include "errno_error.h"

#include <memory>
#include <cstdlib>
#include <ctime>
#include <cstring>

#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace app
{

///////////////////////////////////////////////////////////////////////////////////////////////////
process::~process()
{
    if(running())
    {
        terminate();
        if(!wait_for(std::chrono::seconds(3))) kill();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
static inline void discard(int fd)
{
    if(fd != -1)
    {
        close(fd);
        fd= -1;
    }
}

static inline void discard(int fd[2])
{
    discard(fd[0]);
    discard(fd[1]);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
static void pipe_if(bool cond, int fd[2])
{
    if(cond && pipe2(fd, O_NONBLOCK)) throw errno_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
static void dup_if(bool cond, int fd_io, int fd_rep, int fd_x)
{
    if(cond && dup2(fd_rep, fd_io) == -1) throw errno_error();
    discard(fd_rep);
    discard(fd_x);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
static void open_if(bool cond, std::basic_ios<char>& stream, stdio_filebuf<char>& buf, std::ios_base::openmode mode, int fd_con, int fd_x)
{
    if(cond)
    {
        if(!buf.open(fd_con, mode)) throw errno_error();
        stream.rdbuf(&buf);
    }
    else discard(fd_con);

    discard(fd_x);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void process::_M_process(std::function<int()> func, bool group, redir_flags flags)
{
    int out_fd[2]= {-1, -1}, in_fd[2]= {-1, -1}, err_fd[2]= {-1, -1};

    try
    {
        pipe_if(flags & redir::out, out_fd);
        pipe_if(flags & redir::in, in_fd);
        pipe_if(flags & redir::err, err_fd);

        _M_id= fork();
        if(_M_id == -1) throw errno_error();

        if(_M_id == 0)
        {
            dup_if(flags & redir::out, STDOUT_FILENO, out_fd[1], out_fd[0]);
            dup_if(flags & redir::in, STDIN_FILENO, in_fd[0], in_fd[1]);
            dup_if(flags & redir::err, STDERR_FILENO, err_fd[1], err_fd[0]);

            int code= func();
            exit(code);
        }

        open_if(flags & redir::out, cout, _M_cout, std::ios_base::in, out_fd[0], out_fd[1]);
        open_if(flags & redir::in, cin, _M_cin, std::ios_base::out, in_fd[1], in_fd[0]);
        open_if(flags & redir::err, cerr, _M_cerr, std::ios_base::in, err_fd[0], err_fd[1]);

        if(group)
        {
            if(setpgid(_M_id, _M_id)) throw errno_error();
            _M_group= true;
        }

        _M_active= true;
    }
    catch(...)
    {
        discard(out_fd);
        discard(in_fd);
        discard(err_fd);

        throw;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool process::running()
{
    while(_M_active)
    {
        int code;
        id x= waitpid(_M_group? -_M_id: _M_id, &code, WNOHANG);
        if(x == -1)
        {
            if(std::errc(errno) == std::errc::no_child_process)
                _M_active= false;
            else throw errno_error();
        }
        else if(x == 0)
            break;
        else if(x == _M_id) set_code(code);
    }
    return _M_active;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void process::set_code(int code)
{
    if(WIFEXITED(code))
        _M_code= app::exit_code(WEXITSTATUS(code));
    else if(WIFSIGNALED(code))
        _M_code= app::exit_code(static_cast<app::signal>(WTERMSIG(code)));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool process::signal(app::signal x)
{
    if(_M_active)
    {
        int code= ::kill(_M_group? -_M_id: _M_id, int(x));
        if(code == -1)
        {
            if(std::errc(errno) == std::errc::no_such_process)
                return false;
            else throw errno_error();
        }
        return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
static void handler(int)
{
    return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool process::_M_wait_for(std::chrono::seconds s, std::chrono::nanoseconds ns)
{
    if(running())
    {
        struct sigaction sa_old, sa_new;
        sa_new.sa_handler= handler;
        sigemptyset(&sa_new.sa_mask);
        sa_new.sa_flags=0;

        if(sigaction(int(app::signal::child), &sa_new, &sa_old)) throw errno_error();

        struct timespec x=
        {
            static_cast<std::time_t>(s.count()),
            static_cast<long>(ns.count())
        };

        bool value= false;
        while(nanosleep(&x, &x) == -1)
        {
            if(std::errc(errno) == std::errc::interrupted)
            {
                if(!running())
                {
                    value= true;
                    break;
                }
            }
            else throw errno_error();
        }

        if(sigaction(int(app::signal::child), &sa_old, nullptr)) throw errno_error();
        return value;
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void process::join()
{
    while(_M_active)
    {
        int code;
        id x= waitpid(_M_group? -_M_id: _M_id, &code, 0);
        if(x == -1)
        {
            if(std::errc(errno) == std::errc::no_child_process)
                _M_active= false;
            else throw errno_error();
        }
        else if(x == _M_id) set_code(code);
    }
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
exit_code execute(const std::string& command)
{
    int code= system(command.data());
    switch(code)
    {
    case  -1: throw execute_error("app::execute failed");
    case 127: throw execute_error("Could not execute shell in the child process");

    default:
        if(WIFEXITED(code))
            return exit_code(WEXITSTATUS(code));
        else if(WIFSIGNALED(code))
            return exit_code(static_cast<app::signal>(WTERMSIG(code)));
        else return exit_code();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void internal::sleep_for(std::chrono::seconds s, std::chrono::nanoseconds ns)
{
    struct timespec x=
    {
        static_cast<std::time_t>(s.count()),
        static_cast<long>(ns.count())
    };

    nanosleep(&x, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}
