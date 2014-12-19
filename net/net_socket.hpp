///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef NET_SOCKET_HPP
#define NET_SOCKET_HPP

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "socket/socket.hpp"
#include "address.hpp"

#include <string>
#include <netinet/in.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace net
{

///////////////////////////////////////////////////////////////////////////////////////////////////
typedef in_port_t port;

///////////////////////////////////////////////////////////////////////////////////////////////////
class socket: public app::socket
{
public:
    typedef app::socket::id id;
    typedef app::socket::type type;

public:
    socket() noexcept = default;
    socket(const socket&) = delete;
    socket(socket&& x) noexcept { swap(x); }

    socket(type);

    socket& operator=(const socket&) = delete;
    socket& operator=(socket&& x) noexcept
    {
        swap(x);
        return (*this);
    }

    void bind(net::address address, net::port port);
    void bind(net::port port) { bind(net::address::any, port); }

    void connect(net::address address, net::port port);

    void set_multicast_loop(bool);
    void set_multicast_ttl(int);
    void set_multicast_all(bool);

    void add_membership(net::address group);
    void drop_membership(net::address group);

    size_t send_to(net::address address, net::port port, const std::string& string, bool wait = true)
        { return send_to(address, port, string.data(), string.size(), wait); }
    size_t send_to(net::address address, net::port port, const void* buffer, size_t n, bool wait = true);

    size_t recv_from(net::address& address, net::port& port, std::string& string, size_t max, bool wait = true);
    size_t recv_from(net::address& address, net::port& port, void* buffer, size_t n, bool wait = true);

private:
    sockaddr_in from(net::address, net::port);
    using base = app::socket;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // NET_SOCKET_HPP
