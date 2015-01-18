///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef FILE_HPP
#define FILE_HPP

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "enum.hpp"
#include "perm.hpp"

#include <chrono>
#include <cstdint>
#include <string>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace storage
{

///////////////////////////////////////////////////////////////////////////////////////////////////
enum class type
{
    none  = 000,
    fifo  = 001,
    chr   = 002,
    dir   = 004,
    block = 006,
    file  = 010,
    link  = 012,
    sock  = 014
};

///////////////////////////////////////////////////////////////////////////////////////////////////
typedef off_t offset;

typedef uid_t uid;
typedef gid_t gid;

///////////////////////////////////////////////////////////////////////////////////////////////////
enum class open
{
    read = O_RDONLY,
    write = O_WRONLY,
    read_write = O_RDWR,
};

///////////////////////////////////////////////////////////////////////////////////////////////////
enum class open_opt
{
    none      = 0,
    create    = O_CREAT,
    trunc     = O_TRUNC,
    append    = O_APPEND,
    sync      = O_SYNC,
    direct    = O_DIRECT,
    non_block = O_NONBLOCK,
};
DECLARE_OPERATOR(open_opt)

///////////////////////////////////////////////////////////////////////////////////////////////////
enum class origin
{
    beg = SEEK_SET,
    cur = SEEK_CUR,
    end = SEEK_END,
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class file
{
public:
    typedef int id;
    static constexpr id invalid = -1;

public:
    file() noexcept = default;
    file(const file&) = delete;
    file(file&& x) noexcept { swap(x); }

    file(const std::string& name,
         storage::open,
         storage::open_opt = open_opt::none,
    storage::perm = user_read_write | group_read | other_read);

    ~file() { close(); }

    void close() noexcept;
    bool is_open() const noexcept { return _M_fd != invalid; }

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

    size_t write(const std::string& string)
        { return write(string.data(), string.size()); }
    size_t write(const void* buffer, size_t n);

    size_t read(std::string& string, size_t max, bool wait = true);
    size_t read(void* buffer, size_t max, bool wait = true);

    std::string readline(bool wait = true, char delim = '\n');
    bool getline(std::string& string, bool wait = true, char delim = '\n');
    bool eof();

    storage::offset seek(storage::offset, storage::origin = origin::beg);
    storage::offset tell() { return seek(0, origin::cur); }
    storage::offset size();

    void truncate(storage::offset length);

    template<typename Rep, typename Period>
    bool can_read(const std::chrono::duration<Rep, Period>& x)
    {
        std::chrono::seconds s = std::chrono::duration_cast<std::chrono::seconds>(x);
        std::chrono::nanoseconds n = std::chrono::duration_cast<std::chrono::nanoseconds>(x - s);
        return can_read(s, n);
    }

    template<typename Rep, typename Period>
    bool can_write(const std::chrono::duration<Rep, Period>& x)
    {
        std::chrono::seconds s = std::chrono::duration_cast<std::chrono::seconds>(x);
        std::chrono::nanoseconds n = std::chrono::duration_cast<std::chrono::nanoseconds>(x - s);
        return can_write(s, n);
    }

    file::id get_id() const { return _M_fd; }

    template<typename T>
    int control(int request, T value)
    {
        return _M_control(request, reinterpret_cast<void*>(value));
    }

    int control(int request) { return _M_control(request, nullptr); }

protected:
    file::id _M_fd = invalid;

    bool can_read(std::chrono::seconds, std::chrono::nanoseconds);
    bool can_write(std::chrono::seconds, std::chrono::nanoseconds);

    int _M_control(unsigned long request, void* buffer);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void remove(const std::string& name);
void rename(const std::string& prev, const std::string& name);
std::string real_path(const std::string& path);

void chown(const std::string& name, storage::uid, storage::gid, bool deref = true);
void chmod(const std::string& name, storage::perm);

void mkdir(const std::string& name, storage::perm = perm::user_all | perm::group_read | perm::group_exec | perm::other_read | perm::other_exec);
void rmdir(const std::string& name);

bool exists(const std::string& name) noexcept;
storage::type get_type(const std::string& name) noexcept;

inline bool is_fifo (const std::string& name) noexcept { return get_type(name) == storage::type::fifo ; }
inline bool is_chr  (const std::string& name) noexcept { return get_type(name) == storage::type::chr  ; }
inline bool is_dir  (const std::string& name) noexcept { return get_type(name) == storage::type::dir  ; }
inline bool is_block(const std::string& name) noexcept { return get_type(name) == storage::type::block; }
inline bool is_file (const std::string& name) noexcept { return get_type(name) == storage::type::file ; }
inline bool is_link (const std::string& name) noexcept { return get_type(name) == storage::type::link ; }
inline bool is_sock (const std::string& name) noexcept { return get_type(name) == storage::type::sock ; }

storage::offset size(const std::string& name);

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // FILE_HPP
