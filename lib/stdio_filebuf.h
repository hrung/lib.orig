///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef STDIO_FILEBUF_H
#define STDIO_FILEBUF_H

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "basic_filebuf.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace app
{

///////////////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Traits = std::char_traits<CharT> >
class stdio_filebuf : public basic_filebuf<CharT, Traits>
{
public:
    typedef CharT                            char_type;
    typedef Traits                           traits_type;
    typedef typename traits_type::int_type   int_type;
    typedef typename traits_type::pos_type   pos_type;
    typedef typename traits_type::off_type   off_type;
    typedef typename traits_type::state_type state_type;

    stdio_filebuf() : basic_filebuf<CharT, Traits>() { }
    stdio_filebuf(stdio_filebuf&& rhs) : basic_filebuf<CharT, Traits>(std::move(rhs)) { }
    virtual ~stdio_filebuf();

    stdio_filebuf& operator=(stdio_filebuf&& rhs)
    {
        basic_filebuf<CharT, Traits>::operator=(std::move(rhs));
        return *this;
    }
    void swap(stdio_filebuf& rhs) { basic_filebuf<CharT, Traits>::swap(rhs); }

    stdio_filebuf* open(int fd, std::ios_base::openmode mode);
    stdio_filebuf* open(FILE* f, std::ios_base::openmode mode);

    FILE* file() { return this->_M_file; }
    int fd() { return fileno(this->_M_file); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
template <class CharT, class Traits>
stdio_filebuf<CharT, Traits>::~stdio_filebuf()
{ }

///////////////////////////////////////////////////////////////////////////////////////////////////
template <class CharT, class Traits>
inline
void
swap(stdio_filebuf<CharT, Traits>& x, stdio_filebuf<CharT, Traits>& y)
{
    x.swap(y);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <class CharT, class Traits>
stdio_filebuf<CharT, Traits>*
stdio_filebuf<CharT, Traits>::open(int fd, std::ios_base::openmode mode)
{
    const char* md= this->_M_get_mode(mode);
    if (md)
    {
        this->_M_file= fdopen(fd, md);
        this->_M_om = mode;
        return this;
    }
    else return nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <class CharT, class Traits>
stdio_filebuf<CharT, Traits>*
stdio_filebuf<CharT, Traits>::open(FILE* f, std::ios_base::openmode mode)
{
    this->_M_file = f;
    this->_M_om = mode;
    return this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // STDIO_FILEBUF_H
