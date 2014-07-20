///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "file.h"
#include "except.h"
#include <memory>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/select.h>
#include <limits.h> // PATH_MAX

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace storage
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void file::open(const std::string& name, open_flags flags, perm perm)
{
    FUNCTION_CONTEXT(ctx);

    int mode= flags & ~open::read_write;
    if(flags.contains(open::read_write))
        mode|= O_RDWR;
    else if(flags.contains(open::write))
        mode|= O_WRONLY;
    else mode|= O_RDONLY;

    _M_fd= ::open(name.data(), mode, perm);
    if(_M_fd == invalid_desc) throw system_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void file::close()
{
    if(!(_M_fd==0 || _M_fd==1 || _M_fd==2) && is_open())
    {
        ::close(_M_fd);
        _M_fd= invalid_desc;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t file::write(const void* buffer, size_t n)
{
    FUNCTION_CONTEXT(ctx);

    ssize_t count= ::write(_M_fd, buffer, n);
    if(count == -1) throw system_error();

    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t file::read(std::string& string, size_t max, bool wait)
{
    FUNCTION_CONTEXT(ctx);

    std::unique_ptr<char[]> buffer(new char[max+1]);
    ssize_t count= read(buffer.get(), max, wait);
    buffer[count]=0;

    string.assign(buffer.get(), count);
    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t file::read(void* buffer, size_t max, bool wait)
{
    FUNCTION_CONTEXT(ctx);

    ssize_t count=0;
    if(wait || can_read()) count= ::read(_M_fd, buffer, max);

    if(count == -1) throw system_error();
    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
off_t file::seek(off_t value, storage::origin origin)
{
    FUNCTION_CONTEXT(ctx);

    off_t offset= ::lseek(_M_fd, value, int(origin));
    if(offset == -1) throw system_error();

    return offset;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
off_t file::size()
{
    off_t n= tell();
    off_t e= seek(0, origin::end);

    seek(n);
    return e;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool file::can_read(timeval* tv)
{
    FUNCTION_CONTEXT(ctx);

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(_M_fd, &fds);

    int n= select(_M_fd+1, &fds, 0, 0, tv);
    if(n == -1) throw system_error();

    return n;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool file::can_write(timeval* tv)
{
    FUNCTION_CONTEXT(ctx);

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(_M_fd, &fds);

    int n= select(_M_fd+1, 0, &fds, 0, tv);
    if(n == -1) throw system_error();

    return n;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void remove(const std::string& name)
{
    FUNCTION_CONTEXT(ctx);
    if( ::remove(name.data()) ) throw system_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void rename(const std::string& prev, const std::string& name)
{
    FUNCTION_CONTEXT(ctx);
    if(::rename(prev.data(), name.data())) throw system_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string real_path(const std::string& path)
{
    FUNCTION_CONTEXT(ctx);

    std::unique_ptr<char[]> buffer(new char[PATH_MAX]);
    std::string real;

    if(::realpath(path.data(), buffer.get()))
        real.assign(buffer.get());
    else throw system_error();

    return real;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool stat(const std::string& name, struct stat& value)
{
    return ::stat(name.data(), &value)==0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void chown(const std::string& name, uid_t uid, gid_t gid, bool deref)
{
    FUNCTION_CONTEXT(ctx);
    if( (deref? ::chown(name.data(), uid, gid): lchown(name.data(), uid, gid)) ) throw system_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
mode_t mode(const std::string& name)
{
    struct stat value;
    return stat(name, value)? value.st_mode: 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}
