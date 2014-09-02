///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef PROCESS_H
#define PROCESS_H

///////////////////////////////////////////////////////////////////////////////////////////////////
#include <functional>
#include <vector>
#include <map>
#include <stdexcept>
#include <string>
#include <chrono>

#include <signal.h>
#include <sys/types.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace app
{

///////////////////////////////////////////////////////////////////////////////////////////////////
typedef std::map<std::string, std::string> environment;

typedef std::vector<std::string> arguments;

///////////////////////////////////////////////////////////////////////////////////////////////////
enum class signal
{
    hangup    = SIGHUP,
    interrupt = SIGINT,
    quit      = SIGQUIT,
    illegal   = SIGILL,
    abort     = SIGABRT,
    fpe       = SIGFPE,
    kill      = SIGKILL,
    segment   = SIGSEGV,
    pipe      = SIGPIPE,
    alarm     = SIGALRM,
    terminate = SIGTERM,
    user1     = SIGUSR1,
    user2     = SIGUSR2,
    child     = SIGCHLD,
    cont      = SIGCONT,
    stop      = SIGSTOP,
    term_stop = SIGTSTP,
    term_in   = SIGTTIN,
    term_out  = SIGTTOU
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class process
{
public:
    typedef pid_t id;

    process() noexcept = default;
    process(process&) = delete;
    process(const process&) = delete;

    process(process&& x) noexcept { swap(x); }

    template<typename Callable, typename... Args>
    explicit process(Callable&& func, Args&&... args)
    {
        _M_process(std::bind(std::forward<Callable>(func), std::forward<Args>(args)...));
    }

    process& operator=(const process&) = delete;

    process& operator=(process&& x) noexcept
    {
        swap(x);
        return (*this);
    }

    void swap(process& x)
    {
        std::swap(_M_id, x._M_id);
    }

    process::id get_id() const noexcept { return _M_id; }

    bool running(bool group= false);

    int exit_code() const noexcept { return _M_code; }
    app::signal exit_signal() const noexcept { return _M_signal; }

    bool signal(app::signal, bool group= false);

    bool terminate(bool group= false) { return signal(app::signal::terminate, group); }
    bool kill(bool group= false) { return signal(app::signal::kill, group); }

private:
    id _M_id=0;

    int _M_code=0;
    app::signal _M_signal;

    void _M_process(std::function<int()>);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace this_process
{

///////////////////////////////////////////////////////////////////////////////////////////////////
process::id get_id() noexcept;
process::id parent_id() noexcept;

///////////////////////////////////////////////////////////////////////////////////////////////////
int replace(const std::string& path, const arguments& args= {});
int replace_e(const environment&, const std::string& path, const arguments& args= {});

///////////////////////////////////////////////////////////////////////////////////////////////////
class execute_error: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

int execute(const std::string& command, app::signal* = nullptr);

///////////////////////////////////////////////////////////////////////////////////////////////////
void _M_sleep_for(std::chrono::seconds, std::chrono::nanoseconds);

template<typename Rep, typename Period>
inline void sleep_for(const std::chrono::duration<Rep, Period>& t)
{
    std::chrono::seconds s= std::chrono::duration_cast<std::chrono::seconds>(t);
    std::chrono::nanoseconds ns= std::chrono::duration_cast<std::chrono::nanoseconds>(t - s);

    _M_sleep_for(s, ns);
  }

template<typename Clock, typename Duration>
inline void sleep_until(const std::chrono::time_point<Clock, Duration>& t) { sleep_for(t - Clock::now()); }

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // PROCESS_H
