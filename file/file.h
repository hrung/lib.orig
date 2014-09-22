///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef FILE_H
#define FILE_H

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "flags.h"
#include "errno_error.h"

#include <string>
#include <ctime>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace storage
{

///////////////////////////////////////////////////////////////////////////////////////////////////
typedef mode_t perm;
typedef off_t offset;

typedef uid_t uid;
typedef gid_t gid;

///////////////////////////////////////////////////////////////////////////////////////////////////
enum class open
{
    read= 1,
    write= 2,
    read_write= open::read | open::write,

    create= 4,
    truncate= 8
};
DECLARE_FLAGS(open)

///////////////////////////////////////////////////////////////////////////////////////////////////
enum class origin
{
    start= SEEK_SET,
    current= SEEK_CUR,
    end= SEEK_END
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class file
{
public:
    typedef int id;
    static constexpr id invalid= -1;

public:
    file() noexcept = default;
    file(const file&) = delete;
    file(file&& x) noexcept { swap(x); }

    file(const std::string& name, open_flags flags, perm = 0644);
    ~file() { close(); }

    void close() noexcept;

    file& operator=(const file&) = delete;
    file& operator=(file&& x) noexcept
    {
        swap(x);
        return (*this);
    }

    void swap(file& x) noexcept
    {
        std::swap(_M_fd, x._M_fd);
    }

    bool open() const { return _M_fd != invalid; }

    ssize_t write(const std::string& string)
        { return write(string.data(), string.size()); }
    ssize_t write(const void* buffer, size_t n);

    ssize_t read(std::string& string, size_t max, bool wait= true);
    ssize_t read(void* buffer, size_t max, bool wait= true);

    offset seek(offset value, origin = origin::start);
    offset tell() { return seek(0, origin::current); }
    offset size();

    bool can_read(int wait_usec=0) { timeval tv= {0, wait_usec}; return can_read((wait_usec<0)? 0: &tv); }
    bool can_write(int wait_usec=0) { timeval tv= {0, wait_usec}; return can_write((wait_usec<0)? 0: &tv); }

    file::id get_id() const { return _M_fd; }

protected:
    id _M_fd= invalid;

    bool can_read(timeval* tv);
    bool can_write(timeval* tv);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void remove(const std::string& name);
void rename(const std::string& prev, const std::string& name);
std::string real_path(const std::string& path);

bool stat(const std::string& name, struct stat&);
void chown(const std::string& name, storage::uid uid, storage::gid gid, bool deref= true);

perm mode(const std::string& name);
inline bool exists   (const std::string& name) { return mode(name); }
inline bool is_file  (const std::string& name) { return S_ISREG(mode(name)); }
inline bool is_dir   (const std::string& name) { return S_ISDIR(mode(name)); }
inline bool is_char  (const std::string& name) { return S_ISCHR(mode(name)); }
inline bool is_block (const std::string& name) { return S_ISBLK(mode(name)); }
inline bool is_fifo  (const std::string& name) { return S_ISFIFO(mode(name)); }
inline bool is_link  (const std::string& name) { return S_ISLNK(mode(name)); }
inline bool is_socket(const std::string& name) { return S_ISSOCK(mode(name)); }

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif
