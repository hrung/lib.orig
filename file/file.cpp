///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "file.h"

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
file::file(const std::string& name, open_flags flags, storage::perm perm)
{
    int mode= flags & ~open::read_write;
    if(flags.contains(open::read_write))
        mode|= O_RDWR;
    else if(flags.contains(open::write))
        mode|= O_WRONLY;
    else mode|= O_RDONLY;

    _M_fd= ::open(name.data(), mode, perm);
    if(_M_fd == invalid) throw errno_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void file::close() noexcept
{
    if(!(_M_fd==0 || _M_fd==1 || _M_fd==2) && open())
    {
        ::close(_M_fd);
        _M_fd= invalid;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t file::write(const void* buffer, size_t n)
{
    ssize_t count= ::write(_M_fd, buffer, n);
    if(count == -1) throw errno_error();

    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t file::read(std::string& string, size_t max, bool wait)
{
    std::unique_ptr<char[]> buffer(new char[max+1]);
    ssize_t count= read(buffer.get(), max, wait);
    buffer[count]=0;

    string.assign(buffer.get(), count);
    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t file::read(void* buffer, size_t max, bool wait)
{
    ssize_t count=0;
    if(wait || can_read()) count= ::read(_M_fd, buffer, max);

    if(count == -1) throw errno_error();
    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
offset file::seek(offset value, storage::origin origin)
{
    off_t offset= ::lseek(_M_fd, value, int(origin));
    if(offset == -1) throw errno_error();

    return offset;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
offset file::size()
{
    off_t n= tell();
    off_t e= seek(0, origin::end);

    seek(n);
    return e;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool file::can_read(timeval* tv)
{
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(_M_fd, &fds);

    int n= select(_M_fd+1, &fds, 0, 0, tv);
    if(n == -1) throw errno_error();

    return n;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool file::can_write(timeval* tv)
{
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(_M_fd, &fds);

    int n= select(_M_fd+1, 0, &fds, 0, tv);
    if(n == -1) throw errno_error();

    return n;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void remove(const std::string& name)
{
    if( ::remove(name.data()) ) throw errno_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void rename(const std::string& prev, const std::string& name)
{
    if(::rename(prev.data(), name.data())) throw errno_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string real_path(const std::string& path)
{
    std::unique_ptr<char[]> buffer(new char[PATH_MAX]);
    std::string real;

    if(::realpath(path.data(), buffer.get()))
        real.assign(buffer.get());
    else throw errno_error();

    return real;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool stat(const std::string& name, struct stat& x)
{
    return ::stat(name.data(), &x)==0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void chown(const std::string& name, storage::uid uid, storage::gid gid, bool deref)
{
    if( (deref? ::chown(name.data(), uid, gid): lchown(name.data(), uid, gid)) ) throw errno_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
perm mode(const std::string& name)
{
    struct stat value;
    return stat(name, value)? value.st_mode: 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}
