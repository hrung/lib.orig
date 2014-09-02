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
    none      = 0,
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
struct exit_code
{
    static constexpr int none= -1;

    int code() const noexcept { return _M_code; }
    signal term() const noexcept { return _M_term; }

    bool is_none() const noexcept { return _M_code == none && _M_term == app::signal::none; }
    bool is_exit() const noexcept { return _M_code != none; }
    bool is_term() const noexcept { return _M_code == none && _M_term != app::signal::none; }

private:
    int _M_code= none;
    signal _M_term= signal::none;

    friend class process;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class process
{
public:
    typedef pid_t id;

    process() noexcept = default;
    process(process&) = delete;
    process(const process&) = delete;

    process(process&& x) noexcept = default;

    template<typename Callable, typename... Args>
    explicit process(Callable&& func, Args&&... args)
    {
        _M_process(std::bind(std::forward<Callable>(func), std::forward<Args>(args)...), false);
    }

    template<typename Callable, typename... Args>
    explicit process(bool group, Callable&& func, Args&&... args)
    {
        _M_process(std::bind(std::forward<Callable>(func), std::forward<Args>(args)...), group);
    }

    ~process();

    process& operator=(const process&) = delete;
    process& operator=(process&& x) noexcept = default;

    process::id get_id() const noexcept { return _M_id; }

    bool running();
    const app::exit_code& exit_code() const noexcept { return _M_code; }

    bool signal(app::signal);
    bool terminate() { return signal(app::signal::terminate); }
    bool kill() { return signal(app::signal::kill); }

    void detach() noexcept { _M_active= false; }

    template<typename Rep, typename Period>
    bool wait_for(const std::chrono::duration<Rep, Period>& t)
    {
        std::chrono::seconds s= std::chrono::duration_cast<std::chrono::seconds>(t);
        std::chrono::nanoseconds ns= std::chrono::duration_cast<std::chrono::nanoseconds>(t - s);

        return _M_wait_for(s, ns);
    }
    void join();

private:
    id _M_id=0;
    bool _M_active= false;

    bool _M_group= false;

    app::exit_code _M_code;

    void _M_process(std::function<int()>, bool group);
    bool _M_wait_for(std::chrono::seconds, std::chrono::nanoseconds);

    void set_code(int code);
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
