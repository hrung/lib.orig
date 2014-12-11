///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef PROCESS_H
#define PROCESS_H

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "environ.h"
#include "arguments.h"
#include "filebuf.hpp"
#include "enum.hpp"

#include <fstream>
#include <functional>
#include <stdexcept>
#include <string>
#include <chrono>

#include <signal.h>
#include <sys/types.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// GNU libstdc++ currently does not support move semantics for streams (see gcc bug 53626),
/// which will only be available in gcc 5. Therefore, the only way to take advantage of
/// the I/O redirection feature of the process class is to use LLVM's libc++ with the clang++
/// compiler (add -stdlib=libc++ flag). Or wait until gcc 5 is released with new version of the
/// libstdc++.
///
#if !defined(disable_process_redir)
#  if defined(__GNUC__)
#    if !defined(_LIBCPP_VERSION) && (__GNUC__ < 5)
#      define disable_process_redir
#    endif
#  else
#    define disable_process_redir
#  endif
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace app
{

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

    exit_code() noexcept = default;
    exit_code(int code) noexcept: _M_code(code) { }
    exit_code(app::signal term) noexcept: _M_term(term) { }

    int code() const noexcept { return _M_code; }
    signal term() const noexcept { return _M_term; }

    bool is_none() const noexcept { return _M_code == none && _M_term == app::signal::none; }
    bool is_exit() const noexcept { return _M_code != none; }
    bool is_term() const noexcept { return _M_code == none && _M_term != app::signal::none; }

private:
    int _M_code= none;
    signal _M_term= signal::none;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
enum class redir
{
    none=0,
    cout=1,
    cin=2,
    cerr=4,
    all= cout | cin | cerr
};
DECLARE_OPERATOR(redir)

///////////////////////////////////////////////////////////////////////////////////////////////////
class process
{
public:
    typedef pid_t id;
    enum group_t { group };

public:
    process() = default;
    process(const process&) = delete;
    process(process&& x) noexcept { swap(x); }

    ////////////////////
#if !defined(disable_process_redir)
    template<typename Callable, typename... Args>
    process(group_t, app::redir redir, Callable&& func, Args&&... args)
    {
        _M_process(std::bind(std::forward<Callable>(func), std::forward<Args>(args)...), true, redir);
    }
#endif

    ////////////////////
    template<typename Callable, typename... Args>
    process(group_t, Callable&& func, Args&&... args)
    {
        _M_process(std::bind(std::forward<Callable>(func), std::forward<Args>(args)...), true, redir::none);
    }

    ////////////////////
#if !defined(disable_process_redir)
    template<typename Callable, typename... Args>
    process(app::redir redir, Callable&& func, Args&&... args)
    {
        _M_process(std::bind(std::forward<Callable>(func), std::forward<Args>(args)...), false, redir);
    }
#endif

    ////////////////////
    template<typename Callable, typename... Args>
    explicit process(Callable&& func, Args&&... args)
    {
        _M_process(std::bind(std::forward<Callable>(func), std::forward<Args>(args)...), false, redir::none);
    }

    ~process();

    process& operator=(const process&) = delete;
    process& operator=(process&& x) noexcept
    {
        swap(x);
        return (*this);
    }

    void swap(process& x) noexcept
    {
        std::swap(_M_id, x._M_id);
        std::swap(_M_active, x._M_active);
        std::swap(_M_group, x._M_group);
        std::swap(_M_code, x._M_code);

#if !defined(disable_process_redir)
        std::swap(cin, x.cin);
        std::swap(_M_cin, x._M_cin);

        std::swap(cout, x.cout);
        std::swap(_M_cout, x._M_cout);

        std::swap(cerr, x.cerr);
        std::swap(_M_cerr, x._M_cerr);
#endif
    }

    process::id get_id() const noexcept { return _M_id; }

    bool running();
    const app::exit_code& exit_code() const noexcept { return _M_code; }

    bool signal(app::signal);
    bool terminate() { return signal(app::signal::terminate); }
    bool kill() { return signal(app::signal::kill); }

    void detach() noexcept { _M_active= false; }

    template<typename Rep, typename Period>
    bool can_join(const std::chrono::duration<Rep, Period>& x)
    {
        std::chrono::seconds s= std::chrono::duration_cast<std::chrono::seconds>(x);
        std::chrono::nanoseconds n= std::chrono::duration_cast<std::chrono::nanoseconds>(x - s);

        return can_join(s, n);
    }
    void join();

#if !defined(disable_process_redir)
    std::ofstream cin;
    std::ifstream cout, cerr;
#endif

protected:
    id _M_id=0;
    bool _M_active= false;
    bool _M_group= false;

    app::exit_code _M_code;
#if !defined(disable_process_redir)
    filebuf _M_cin, _M_cout, _M_cerr;
#endif

    void _M_process(std::function<int()>, bool group, app::redir);

    bool can_join(std::chrono::seconds, std::chrono::nanoseconds);
    void set_code(int code);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
class execute_error: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace this_process
{

///////////////////////////////////////////////////////////////////////////////////////////////////
process::id get_id() noexcept;
process::id parent_id() noexcept;

///////////////////////////////////////////////////////////////////////////////////////////////////
int replace(const std::string& path, const arguments& args= {});
int replace_e(const environ&, const std::string& path, const arguments& args= {});

///////////////////////////////////////////////////////////////////////////////////////////////////
exit_code execute(const std::string& command);

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace internal { void sleep_for(std::chrono::seconds, std::chrono::nanoseconds); }

template<typename Rep, typename Period>
inline void sleep_for(const std::chrono::duration<Rep, Period>& x)
{
    std::chrono::seconds s= std::chrono::duration_cast<std::chrono::seconds>(x);
    std::chrono::nanoseconds n= std::chrono::duration_cast<std::chrono::nanoseconds>(x - s);

    internal::sleep_for(s, n);
}

template<typename Clock, typename Duration>
inline void sleep_until(const std::chrono::time_point<Clock, Duration>& t) { sleep_for(t - Clock::now()); }

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // PROCESS_H
