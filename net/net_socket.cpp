///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "errno_error.hpp"
#include "net_socket.hpp"

#include <cstring>
#include <memory>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace net
{

///////////////////////////////////////////////////////////////////////////////////////////////////
socket::socket(type x): app::socket(AF_INET, x)
{
    int val = 1;
    if(setsockopt(_M_fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val))) throw errno_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
sockaddr_in socket::from(net::address address, net::port port)
{
    sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_addr = address._M_addr;
    addr.sin_port = htons(port);

    return addr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::bind(net::address address, net::port port)
{
    sockaddr_in addr = from(address, port);
    base::bind((sockaddr*)&addr, sizeof(addr));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::connect(net::address address, net::port port)
{
    sockaddr_in addr = from(address, port);
    base::connect((sockaddr*)&addr, sizeof(addr));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::set_multicast_loop(bool x)
{
    int val = x ? 1 : 0;
    if(setsockopt(_M_fd, IPPROTO_IP, IP_MULTICAST_LOOP, &val, sizeof(val))) throw errno_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::set_multicast_ttl(int x)
{
    if(setsockopt(_M_fd, IPPROTO_IP, IP_MULTICAST_TTL, &x, sizeof(x))) throw errno_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::set_multicast_all(bool x)
{
    int val = x ? 1 : 0;
    if(setsockopt(_M_fd, IPPROTO_IP, IP_MULTICAST_ALL, &val, sizeof(val))) throw errno_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::add_membership(net::address group)
{
    ip_mreq imr;
    imr.imr_multiaddr = group._M_addr;
    imr.imr_interface = net::address::any._M_addr;

    if(setsockopt(_M_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &imr, sizeof(imr))) throw errno_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::drop_membership(net::address group)
{
    ip_mreq imr;
    imr.imr_multiaddr = group._M_addr;
    imr.imr_interface = net::address::any._M_addr;

    if(setsockopt(_M_fd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &imr, sizeof(imr))) throw errno_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
size_t socket::send_to(net::address address, net::port port, const void* buffer, size_t n, bool wait)
{
    sockaddr_in addr = from(address, port);
    return base::send_to((sockaddr*)&addr, sizeof(addr), buffer, n, wait);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
size_t socket::recv_from(net::address& address, net::port& port, std::string& string, size_t max, bool wait)
{
    std::unique_ptr<char[]> buffer(new char[max + 1]);

    ssize_t count = recv_from(address, port, buffer.get(), max, wait);
    buffer[count] = '\0';

    string.assign(buffer.get(), count);
    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
size_t socket::recv_from(net::address& address, net::port& port, void* buffer, size_t n, bool wait)
{
    sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    memset(&addr, 0, addr_len);

    ssize_t count = base::recv_from((sockaddr*)&addr, addr_len, buffer, n, wait);

    if(addr.sin_family == AF_INET)
    {
        address._M_addr = addr.sin_addr;
        port = ntohs(addr.sin_port);
    }

    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}
