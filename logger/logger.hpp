///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LOGGER_H
#define LOGGER_H

///////////////////////////////////////////////////////////////////////////////////////////////////
#include <ostream>
#include <streambuf>
#include <string>

#include <syslog.h>

constexpr char _n = '\n';

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace app
{

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace log
{

///////////////////////////////////////////////////////////////////////////////////////////////////
enum level
{
    emergency = LOG_EMERG,
    alert     = LOG_ALERT,
    critical  = LOG_CRIT,
    error     = LOG_ERR,
    warning   = LOG_WARNING,
    notice    = LOG_NOTICE,
    info      = LOG_INFO,
    debug     = LOG_DEBUG
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class logger_base
{
protected:
    void putchar(char c) noexcept;
    void set_level(app::log::level x) noexcept { level = x; }

    ~logger_base();

private:
    static constexpr app::log::level default_level = info;
    app::log::level level = default_level;

    std::string buffer;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
template< typename CharT, typename Traits = std::char_traits<CharT> >
class logger_streambuf: public std::basic_streambuf<CharT, Traits>, public logger_base
{
public:
    typedef CharT                            char_type;
    typedef Traits                           traits_type;
    typedef typename traits_type::int_type   int_type;
    typedef typename traits_type::pos_type   pos_type;
    typedef typename traits_type::off_type   off_type;
    typedef typename traits_type::state_type state_type;

protected:
    int_type overflow(int_type c = traits_type::eof()) override
    {
        if(!traits_type::eq_int_type(c, traits_type::eof()))
            putchar(traits_type::to_char_type(c));
        return c;
    }

    template<typename, typename> friend class logger_stream;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
template< typename CharT, typename Traits = std::char_traits<CharT> >
class logger_stream: public std::basic_ostream<CharT, Traits>
{
public:
    logger_stream(): std::ostream(&buffer) { }

    logger_stream& operator<<(log::level x) noexcept
    {
        buffer.set_level(x);
        return (*this);
    }

protected:
    logger_streambuf<CharT, Traits> buffer;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern thread_local log::logger_stream<char> logger;

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif
