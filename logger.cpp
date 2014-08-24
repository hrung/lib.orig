///////////////////////////////////////////////////////////////////////////////////////////////////
#include "logger.h"
#include <streambuf>
#include <mutex>
#include <syslog.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
class syslog_streambuf: public std::streambuf
{
protected:
    static constexpr level reset= level::info;
    std::streamsize xsputn(const char* message, std::streamsize n) override
    {
        int pri;
        switch(current)
        {
        case level::emergency:
            pri= LOG_EMERG;
            break;
        case level::alert:
            pri= LOG_ALERT;
            break;
        case level::critical:
            pri= LOG_CRIT;
            break;
        case level::error:
            pri= LOG_ERR;
            break;
        case level::warning:
            pri= LOG_WARNING;
            break;
        case level::notice:
            pri= LOG_NOTICE;
            break;
        case level::info:
            pri= LOG_INFO;
            break;
        case level::debug:
            pri= LOG_DEBUG;
            break;
        }
        syslog(pri, "%s", message);

        current= reset;
        return n;
    }

    level current= reset;
    friend std::ostream& operator<<(std::ostream&, level);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
syslog_streambuf buffer;

///////////////////////////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& stream, level x)
{
    buffer.current= x;
    return stream;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
struct call_once
{
    call_once()
    {
        static std::once_flag once;
        std::call_once(once, []() { std::clog.rdbuf(&buffer); });
    }
}
redir;
