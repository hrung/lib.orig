///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef FILE_H
#define FILE_H

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "flags.h"
#include "except.h"

#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace file
{

///////////////////////////////////////////////////////////////////////////////////////////////////
typedef int desc;
constexpr desc invalid_desc= -1;

typedef mode_t perm;

enum class open
{
    read= 1,
    write= 2,
    read_write= open::read | open::write,

    create=   O_CREAT,
    truncate= O_TRUNC
};
ENUM_FLAGS(open)

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
    file() { }
    file(const std::string& name, open_flags flags) { open(name, flags); }

    virtual ~file() { close(); }

    void open(const std::string& name, open_flags flags, perm = 0644);
    void close();
    bool is_open() const { return _M_fd != invalid_desc; }

    ssize_t write(const std::string& value);
    ssize_t write(const void* p, size_t n);

    ssize_t read(std::string& value, size_t max, bool wait= true);
    ssize_t read(void* pointer, size_t max, bool wait= true);

    off_t seek(off_t value, origin = origin::start);
    off_t tell() { return seek(0, origin::current); }
    off_t size();

    bool can_read(int wait_usec=0) { timeval tv= {0, wait_usec}; return can_read((wait_usec<0)? 0: &tv); }
    bool can_read(timeval* tv);
    bool can_write(int wait_usec=0) { timeval tv= {0, wait_usec}; return can_write((wait_usec<0)? 0: &tv); }
    bool can_write(timeval* tv);

    desc fd() const { return _M_fd; }

    bool stat(struct stat&);
    void chown(uid_t uid, gid_t gid);

    bool is_file()   { return S_ISREG(mode()); }
    bool is_dir()    { return S_ISDIR(mode()); }
    bool is_char()   { return S_ISCHR(mode()); }
    bool is_block()  { return S_ISBLK(mode()); }
    bool is_fifo()   { return S_ISFIFO(mode()); }
    bool is_link()   { return S_ISLNK(mode()); }
    bool is_socket() { return S_ISSOCK(mode()); }

private:
    desc _M_fd= invalid_desc;

    mode_t mode();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void remove(const std::string& name);
void rename(const std::string& prev, const std::string& name);
std::string real_path(const std::string& path);

bool stat(const std::string& name, struct stat&);
void chown(const std::string& name, uid_t uid, gid_t gid, bool deref= true);

mode_t mode(const std::string& name);
inline bool exists(const std::string& name)    { return mode(name); }
inline bool is_file(const std::string& name)   { return S_ISREG(mode(name)); }
inline bool is_dir(const std::string& name)    { return S_ISDIR(mode(name)); }
inline bool is_char(const std::string& name)   { return S_ISCHR(mode(name)); }
inline bool is_block(const std::string& name)  { return S_ISBLK(mode(name)); }
inline bool is_fifo(const std::string& name)   { return S_ISFIFO(mode(name)); }
inline bool is_link(const std::string& name)   { return S_ISLNK(mode(name)); }
inline bool is_socket(const std::string& name) { return S_ISSOCK(mode(name)); }

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif