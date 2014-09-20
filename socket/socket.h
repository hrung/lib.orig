///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef SOCKET_H
#define SOCKET_H

///////////////////////////////////////////////////////////////////////////////////////////////////
#include <chrono>
#include <string>

#include <sys/socket.h>
#include <sys/types.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
struct sockaddr;

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace app
{

///////////////////////////////////////////////////////////////////////////////////////////////////
class socket
{
public:
    typedef int id;
    static constexpr id invalid= -1;

    enum type { stream, datagram };

public:
    socket() noexcept = default;
    socket(const socket&) = delete;
    socket(socket&& x) noexcept { swap(x); }
    virtual ~socket();

    socket& operator=(const socket&) = delete;
    socket& operator=(socket&& x) noexcept
    {
        swap(x);
        return (*this);
    }

    void swap(socket& x) noexcept
    {
        std::swap(_M_fd, x._M_fd);
    }

    void listen(int max= 128);
    void accept(socket& socket);

    void set_non_blocking(bool);

    template<typename Rep, typename Period>
    bool can_recv(const std::chrono::duration<Rep, Period>& x)
    {
        std::chrono::seconds s= std::chrono::duration_cast<std::chrono::seconds>(x);
        std::chrono::nanoseconds n= std::chrono::duration_cast<std::chrono::nanoseconds>(x - s);
        return can_recv(s, n);
    }

    ssize_t send(const std::string& string, bool wait= true)
        { return send(string.data(), string.size(), wait); }
    ssize_t send(const void* buffer, size_t n, bool wait= true);

    ssize_t recv(std::string& string, size_t max, bool wait= true);
    ssize_t recv(void* buffer, size_t max, bool wait= true);

    socket::id get_id() const noexcept { return _M_fd; }

protected:
    id _M_fd= invalid;

    socket(int family, type);

    bool can_recv(std::chrono::seconds, std::chrono::nanoseconds);

    void bind(sockaddr* addr, socklen_t addr_len);
    void connect(sockaddr* addr, socklen_t addr_len);

    ssize_t send_to(sockaddr* addr, socklen_t addr_len, const void* buffer, size_t n, bool wait= true);
    ssize_t recv_from(sockaddr* addr, socklen_t& addr_len, void* buffer, size_t n, bool wait= true);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif
