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

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace app
{

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace log
{

///////////////////////////////////////////////////////////////////////////////////////////////////
enum level
{
    emergency   = LOG_EMERG,
    alert       = LOG_ALERT,
    critical    = LOG_CRIT,
    error       = LOG_ERR,
    warning     = LOG_WARNING,
    notice      = LOG_NOTICE,
    info        = LOG_INFO,
    debug       = LOG_DEBUG
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class logbuf_base
{
protected:
    void putchar(char c);
    void set_level(level x) noexcept { _M_level= x; }

    ~logbuf_base();

private:
    static constexpr level default_level= info;
    level _M_level= default_level;

    static constexpr char endl= '\n';
    std::string _M_buffer;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
template< typename CharT, typename Traits= std::char_traits<CharT> >
class basic_logbuf: public std::basic_streambuf<CharT, Traits>, public logbuf_base
{
public:
    typedef CharT                            char_type;
    typedef Traits                           traits_type;
    typedef typename traits_type::int_type   int_type;
    typedef typename traits_type::pos_type   pos_type;
    typedef typename traits_type::off_type   off_type;
    typedef typename traits_type::state_type state_type;

protected:
    int_type overflow(int_type c= traits_type::eof()) override
    {
        if(!traits_type::eq_int_type(c, traits_type::eof()))
            putchar(traits_type::to_char_type(c));
        return c;
    }

    template<typename, typename> friend class basic_stream;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
template< typename CharT, typename Traits= std::char_traits<CharT> >
class basic_stream: public std::basic_ostream<CharT, Traits>
{
public:
    basic_stream(): std::ostream(&_M_buffer) { }

    basic_stream& operator<<(level x)
    {
        _M_buffer.set_level(x);
        return (*this);
    }

protected:
    basic_logbuf<CharT, Traits> _M_buffer;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
typedef basic_stream<char> stream;

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern log::stream logger;

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif
