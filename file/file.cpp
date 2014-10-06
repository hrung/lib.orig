///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "file.h"
#include "errno_error.h"

#include <memory>
#include <cstdlib>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <limits.h> // PATH_MAX
#include <sys/ioctl.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace storage
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
file::file(const std::string& name, storage::open open, open_opt opt, storage::perm perm)
{
    int val;
/*  switch(open)
    {
    case open::read_write:
        val= O_RDWR;
        break;
    case open::write:
        val= O_WRONLY;
        break;
    default:
        val= O_RDONLY;
        break;
    }
*/  val= static_cast<int>(open);

    if(opt && open_opt::create) val|= O_CREAT;
    if(opt && open_opt::trunc)  val|= O_TRUNC;
    if(opt && open_opt::append) val|= O_APPEND;

    _M_fd= ::open(name.data(), val, static_cast<mode_t>(perm));
    if(_M_fd == invalid) throw errno_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void file::close() noexcept
{
    if(is_open())
    {
        if(!(_M_fd == STDIN_FILENO || _M_fd == STDOUT_FILENO || _M_fd == STDERR_FILENO))
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
    if(wait || can_read(std::chrono::seconds(0))) count= ::read(_M_fd, buffer, max);

    if(count == -1) throw errno_error();
    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
offset file::seek(storage::offset offset, storage::origin origin)
{
    int val;
/*  switch(origin)
    {
    case origin::beg:
        val= SEEK_SET;
        break;
    case origin::cur:
        val= SEEK_CUR;
        break;
    case origin::end:
        val= SEEK_END;
        break;
    }
*/  val= static_cast<int>(origin);

    storage::offset n= ::lseek(_M_fd, offset, val);
    if(n == -1) throw errno_error();

    return n;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
offset file::size()
{
    offset n= tell();
    offset e= seek(0, origin::end);

    seek(n);
    return e;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool file::can_read(std::chrono::seconds s, std::chrono::nanoseconds n)
{
    timespec time= { static_cast<std::time_t>(s.count()), static_cast<long>(n.count()) };

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(_M_fd, &fds);

    int count= pselect(_M_fd+1, &fds, 0, 0, &time, nullptr);
    if(count == -1) throw errno_error();

    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool file::can_write(std::chrono::seconds s, std::chrono::nanoseconds n)
{
    timespec time= { static_cast<std::time_t>(s.count()), static_cast<long>(n.count()) };

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(_M_fd, &fds);

    int count= pselect(_M_fd+1, 0, &fds, 0, &time, nullptr);
    if(count == -1) throw errno_error();

    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int file::control(int request, void* buffer)
{
    int val= ::ioctl(_M_fd, request, buffer);
    if(val == -1) throw errno_error();

    return val;
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
void chown(const std::string& name, storage::uid uid, storage::gid gid, bool deref)
{
    if( (deref? ::chown(name.data(), uid, gid): lchown(name.data(), uid, gid)) ) throw errno_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void chmod(const std::string& name, storage::perm perm)
{
    if(::chmod(name.data(), static_cast<mode_t>(perm))) throw errno_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mkdir(const std::string& name, storage::perm perm)
{
    if(::mkdir(name.data(), static_cast<mode_t>(perm))) throw errno_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool exists(const std::string& name) noexcept
{
    struct stat x;
    return !::stat(name.data(), &x);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
storage::type get_type(const std::string& name) noexcept
{
    struct stat x;
    return ::stat(name.data(), &x)?
        storage::type::none:
    storage::type((x.st_mode & S_IFMT) >> 12);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
storage::offset size(const std::string& name)
{
    struct stat x;
    if(::stat(name.data(), &x)) throw errno_error();

    return x.st_size;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}
