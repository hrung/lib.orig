///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef FILEBUF_HPP
#define FILEBUF_HPP

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(__GLIBCXX__)

// GNU libstdc++ extension provides stdio_filebuf class
// for working with standard C FILE* and POSIX file descriptors.

#include <ext/stdio_filebuf.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace app
{
typedef __gnu_cxx::stdio_filebuf<char> filebuf;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
#elif defined(_LIBCPP_VERSION)

// LLVM libc++ basic_filebuf defines FILE* __file_ as a private member variable,
// so we have to use a nasty hack to get access to it in the derived class.

#define private protected
#include <fstream>
#undef private

#include <cstdio>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace app
{

///////////////////////////////////////////////////////////////////////////////////////////////////
class filebuf: public std::basic_filebuf<char>
{
public:
    filebuf(): std::basic_filebuf<char>() { }
    filebuf(filebuf&& x): std::basic_filebuf<char>(std::move(x)) { }
    ~filebuf() override { }

    filebuf(int fd, std::ios_base::openmode mode): filebuf()
    {
        open(fd, mode);
    }
    filebuf(FILE* f, std::ios_base::openmode mode): filebuf()
    {
        open(f, mode);
    }

    filebuf& operator=(filebuf&& x)
    {
        std::basic_filebuf<char>::operator=(std::move(x));
        return (*this);
    }
    void swap(basic_filebuf& x)
    {
        std::basic_filebuf<char>::swap(x);
    }

    filebuf* open(int fd, std::ios_base::openmode mode);
    filebuf* open(FILE* f, std::ios_base::openmode mode);

    FILE* file() noexcept { return __file_; }
    int fd() noexcept { return fileno(__file_); }

protected:
    const char* get_mode(std::ios_base::openmode mode);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
inline const char* filebuf::get_mode(std::ios_base::openmode mode)
{
    switch ((mode & ~std::ios_base::ate) | 0)
    {
    case std::ios_base::out:
    case std::ios_base::out | std::ios_base::trunc:
        return "w";
    case std::ios_base::out | std::ios_base::app:
    case std::ios_base::app:
        return "a";
        break;
    case std::ios_base::in:
        return "r";
    case std::ios_base::in  | std::ios_base::out:
        return "r+";
    case std::ios_base::in  | std::ios_base::out | std::ios_base::trunc:
        return "w+";
    case std::ios_base::in  | std::ios_base::out | std::ios_base::app:
    case std::ios_base::in  | std::ios_base::app:
        return "a+";
    case std::ios_base::out | std::ios_base::binary:
    case std::ios_base::out | std::ios_base::trunc | std::ios_base::binary:
        return "wb";
    case std::ios_base::out | std::ios_base::app | std::ios_base::binary:
    case std::ios_base::app | std::ios_base::binary:
        return "ab";
    case std::ios_base::in  | std::ios_base::binary:
        return "rb";
    case std::ios_base::in  | std::ios_base::out | std::ios_base::binary:
        return "r+b";
    case std::ios_base::in  | std::ios_base::out | std::ios_base::trunc | std::ios_base::binary:
        return "w+b";
    case std::ios_base::in  | std::ios_base::out | std::ios_base::app | std::ios_base::binary:
    case std::ios_base::in  | std::ios_base::app | std::ios_base::binary:
        return "a+b";
    default:
        return nullptr;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
inline filebuf* filebuf::open(int fd, std::ios_base::openmode mode)
{
    const char* md = get_mode(mode);
    if(md)
    {
        __file_ = fdopen(fd, md);
        __om_ = mode;
        return this;
    }
    else return nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
inline filebuf* filebuf::open(FILE* f, std::ios_base::openmode mode)
{
    __file_ = f;
    __om_ = mode;
    return this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
inline void swap(filebuf& x, filebuf& y)
{
    x.swap(y);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#else
#  error "Unsupported Standard C++ Library"
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // FILEBUF_HPP
