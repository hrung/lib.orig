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
        syslog(static_cast<int>(current), "%s", message);

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
