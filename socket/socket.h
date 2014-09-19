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
#include "address.h"

#include <string>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace net
{

///////////////////////////////////////////////////////////////////////////////////////////////////
typedef in_port_t port;

typedef int desc;
constexpr desc invalid_desc= -1;

///////////////////////////////////////////////////////////////////////////////////////////////////
enum class type
{
    stream= SOCK_STREAM,
    datagram= SOCK_DGRAM
};

///////////////////////////////////////////////////////////////////////////////////////////////////
enum class family
{
    local= AF_UNIX,
    net= AF_INET
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class socket
{
public:
    socket() noexcept = default;
    socket(const socket&) = delete;
    socket(socket&& x) noexcept { swap(x); }

    socket(net::family, net::type);
    virtual ~socket();

    socket& operator=(const socket&) = delete;
    socket& operator=(socket&& x) noexcept
    {
        swap(x);
        return (*this);
    }

    void swap(socket& x) noexcept
    {
        std::swap(_M_family, x._M_family);
        std::swap(_M_fd, x._M_fd);
    }

    void bind(const std::string& path);
    void bind(ipv4::address address, net::port port);
    void bind(net::port port) { bind(ipv4::address::any, port); }

    void connect(const std::string& path);
    void connect(ipv4::address address, net::port port);

    void listen(int max= 128);
    void accept(socket& socket);

    void set_non_blocking(bool);
    bool can_recv(int wait_usec=0) { timeval tv={0, wait_usec}; return can_recv((wait_usec<0)? 0: &tv); }
    bool can_recv(timeval* tv);

    void set_multicast_loop(bool);
    void set_multicast_ttl(unsigned char);
    void set_multicast_all(bool);

    void add_membership(ipv4::address group);
    void drop_membership(ipv4::address group);

    ssize_t send(const std::string& string, bool wait= true)
        { return send(string.data(), string.size(), wait); }
    ssize_t send(const void* buffer, size_t n, bool wait= true);

    ssize_t sendto(const std::string& path, const std::string& string, bool wait= true)
        { return sendto(path, string.data(), string.size(), wait); }
    ssize_t sendto(const std::string& path, const void* buffer, size_t n, bool wait= true);

    ssize_t sendto(ipv4::address address, net::port port, const std::string& string, bool wait= true)
        { return sendto(address, port, string.data(), string.size(), wait); }
    ssize_t sendto(ipv4::address address, net::port port, const void* buffer, size_t n, bool wait= true);

    ssize_t recv(std::string& string, size_t max, bool wait= true);
    ssize_t recv(void* buffer, size_t max, bool wait= true);

    ssize_t recvfrom(std::string& path, std::string& string, size_t max, bool wait= true);
    ssize_t recvfrom(std::string& path, void* buffer, size_t n, bool wait= true);

    ssize_t recvfrom(ipv4::address& address, net::port& port, std::string& string, size_t max, bool wait= true);
    ssize_t recvfrom(ipv4::address& address, net::port& port, void* buffer, size_t n, bool wait= true);

    net::family family() const { return _M_family; }
    net::desc desc() const { return _M_fd; }

protected:
    net::family _M_family;
    net::desc _M_fd= invalid_desc;

    sockaddr_un from(const std::string&);
    sockaddr_in from(ipv4::address, net::port);

    void bind(sockaddr* addr, socklen_t addr_len);
    void connect(sockaddr* addr, socklen_t addr_len);

    ssize_t sendto(sockaddr* addr, socklen_t addr_len, const void* buffer, size_t n, bool wait= true);
    ssize_t recvfrom(sockaddr* addr, socklen_t& addr_len, void* buffer, size_t n, bool wait= true);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif
