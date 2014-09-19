///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "socket.h"
#include "errno_error.h"

#include <memory>

#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <memory.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace net
{

///////////////////////////////////////////////////////////////////////////////////////////////////
socket::socket(net::family family, net::type type)
{
    _M_family= family;
    _M_fd= ::socket(static_cast<int>(_M_family), static_cast<int>(type), 0);
    if(_M_fd == invalid_desc) throw errno_error();

    if(_M_family==family::net)
    {
        int value=1;
        if(setsockopt(_M_fd, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value))) throw errno_error();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
socket::~socket()
{
    if(_M_fd != invalid_desc) ::close(_M_fd);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
sockaddr_un socket::from(const std::string& path)
{
    sockaddr_un addr;

    addr.sun_family= AF_UNIX;
    size_t count= path.copy(addr.sun_path, sizeof(addr.sun_path)-1);
    addr.sun_path[count]=0;

    return addr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
sockaddr_in socket::from(ipv4::address address, net::port port)
{
    sockaddr_in addr;

    addr.sin_family= AF_INET;
    addr.sin_addr= address._M_addr;
    addr.sin_port= htons(port);

    return addr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::bind(sockaddr* addr, socklen_t addr_len)
{
    if(::bind(_M_fd, addr, addr_len)) throw errno_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::bind(const std::string& path)
{
    sockaddr_un addr= from(path);
    bind((sockaddr*)&addr, sizeof(addr));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::bind(ipv4::address address, net::port port)
{
    sockaddr_in addr= from(address, port);
    bind((sockaddr*)&addr, sizeof(addr));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::connect(sockaddr* addr, socklen_t addr_len)
{
    if(::connect(_M_fd, addr, addr_len)) throw errno_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::connect(const std::string& path)
{
    sockaddr_un addr= from(path);
    connect((sockaddr*)&addr, sizeof(addr));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::connect(ipv4::address address, net::port port)
{
    sockaddr_in addr= from(address, port);
    connect((sockaddr*)&addr, sizeof(addr));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::listen(int max)
{
    if(::listen(_M_fd, max)) throw errno_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::accept(net::socket& socket)
{
    socket._M_fd= ::accept(_M_fd, 0, 0);
    if(socket._M_fd <= 0) throw errno_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::set_non_blocking(bool value)
{
    int opt= fcntl(_M_fd, F_GETFL);
    if(opt < 0) throw errno_error();

    if(value)
        opt|= O_NONBLOCK;
    else
        opt&= ~O_NONBLOCK;

    if(fcntl(_M_fd, F_SETFL, opt)) throw errno_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool socket::can_recv(timeval* tv)
{
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(_M_fd, &fds);

    int cnt= select(_M_fd+1, &fds, 0, 0, tv);
    if(cnt == -1) throw errno_error();

    return cnt;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::set_multicast_loop(bool value)
{
    unsigned char opt= (value? 1: 0);
    if(setsockopt(_M_fd,
        IPPROTO_IP,
        IP_MULTICAST_LOOP,
        &opt,
        sizeof(opt)))
    throw errno_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::set_multicast_ttl(unsigned char value)
{
    if(setsockopt(_M_fd,
        IPPROTO_IP,
        IP_MULTICAST_LOOP,
        &value,
        sizeof(value)))
    throw errno_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::set_multicast_all(bool value)
{
    unsigned char opt= (value? 1: 0);
    if(setsockopt(_M_fd,
        IPPROTO_IP,
        IP_MULTICAST_ALL,
        &opt,
        sizeof(opt)))
    throw errno_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::add_membership(ipv4::address group)
{
    ip_mreq imr;
    imr.imr_multiaddr= group._M_addr;
    imr.imr_interface= ipv4::address::any._M_addr;

    if(setsockopt(_M_fd,
        IPPROTO_IP,
        IP_ADD_MEMBERSHIP,
        &imr,
        sizeof(imr)))
    throw errno_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::drop_membership(ipv4::address group)
{
    ip_mreq imr;
    imr.imr_multiaddr= group._M_addr;
    imr.imr_interface= ipv4::address::any._M_addr;

    if(setsockopt(_M_fd,
        IPPROTO_IP,
        IP_DROP_MEMBERSHIP,
        &imr,
        sizeof(imr)))
    throw errno_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t socket::send(const void* buffer, size_t n, bool wait)
{
    ssize_t count= ::send(_M_fd, buffer, n, wait? 0: MSG_DONTWAIT);
    if(count == -1)
    {
        if(!wait && (errno==EAGAIN || errno==EWOULDBLOCK))
            return 0;
        else throw errno_error();
    }
    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t socket::sendto(sockaddr* addr, socklen_t addr_len, const void* buffer, size_t n, bool wait)
{
    ssize_t count= ::sendto(_M_fd, buffer, n, wait? 0: MSG_DONTWAIT, addr, addr_len);
    if(count == -1)
    {
        if(!wait && (errno==EAGAIN || errno==EWOULDBLOCK))
            return 0;
        else throw errno_error();
    }
    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t socket::sendto(const std::string& path, const void* buffer, size_t n, bool wait)
{
    sockaddr_un addr(from(path));
    return sendto((sockaddr*)&addr, sizeof(addr), buffer, n, wait);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t socket::sendto(ipv4::address address, net::port port, const void* buffer, size_t n, bool wait)
{
    sockaddr_in addr= from(address, port);
    return sendto((sockaddr*)&addr, sizeof(addr), buffer, n, wait);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t socket::recv(std::string& string, size_t max, bool wait)
{
    std::unique_ptr<char[]> buffer(new char[max+1]);

    ssize_t count= recv(buffer.get(), max, wait);
    buffer[count]=0;

    string.assign(buffer.get(), count);
    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t socket::recv(void* buffer, size_t max, bool wait)
{
    ssize_t count= ::recv(_M_fd, buffer, max, wait? 0: MSG_DONTWAIT);
    if(count == -1)
    {
        if(!wait && (errno==EAGAIN || errno==EWOULDBLOCK))
            return 0;
        else throw errno_error();
    }
    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t socket::recvfrom(sockaddr* addr, socklen_t& addr_len, void* buffer, size_t n, bool wait)
{
    ssize_t count= ::recvfrom(_M_fd, buffer, n, wait? 0: MSG_DONTWAIT, addr, &addr_len);
    if(count == -1)
    {
        if(!wait && (errno==EAGAIN || errno==EWOULDBLOCK))
            return 0;
        else throw errno_error();
    }
    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t socket::recvfrom(std::string& path, std::string& string, size_t max, bool wait)
{
    std::unique_ptr<char[]> buffer(new char[max+1]);

    ssize_t count= recvfrom(path, buffer.get(), max, wait);
    buffer[count]=0;

    string.assign(buffer.get(), count);
    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t socket::recvfrom(std::string& path, void* buffer, size_t n, bool wait)
{
    sockaddr_un addr;
    socklen_t addr_len= sizeof(addr);
    memset(&addr, 0, addr_len);

    ssize_t count= recvfrom((sockaddr*)&addr, addr_len, buffer, n, wait);

    if(net::family(addr.sun_family) == net::family::local) path.assign(addr.sun_path);

    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t socket::recvfrom(ipv4::address& address, net::port& port, std::string& string, size_t max, bool wait)
{
    std::unique_ptr<char[]> buffer(new char[max+1]);

    ssize_t count= recvfrom(address, port, buffer.get(), max, wait);
    buffer[count]=0;

    string.assign(buffer.get(), count);
    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t socket::recvfrom(ipv4::address& address, net::port& port, void* buffer, size_t n, bool wait)
{
    sockaddr_in addr;
    socklen_t addr_len= sizeof(addr);
    memset(&addr, 0, addr_len);

    ssize_t count= recvfrom((sockaddr*)&addr, addr_len, buffer, n, wait);

    if(net::family(addr.sin_family) == net::family::net)
    {
        address._M_addr= addr.sin_addr;
        port= ntohs(addr.sin_port);
    }

    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}
