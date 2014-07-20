///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "socket.h"
#include <memory>

#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <memory.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace net
{

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::create(net::family family, net::type type)
{
    FUNCTION_CONTEXT(ctx);

    _M_family= family;
    _M_fd= ::socket(static_cast<int>(_M_family), static_cast<int>(type), 0);
    if(_M_fd == invalid_desc) throw system_error();

    if(_M_family==family::net)
    {
        int value=1;
        if(setsockopt(_M_fd,
            SOL_SOCKET,
            SO_REUSEADDR,
            &value,
            sizeof(value)))
        throw system_error();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::close()
{
    FUNCTION_CONTEXT(ctx);

    if(_M_fd != invalid_desc)
    {
        ::close(_M_fd);
        _M_fd= invalid_desc;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
sockaddr_un socket::from(const std::string& path)
{
    FUNCTION_CONTEXT(ctx);
    sockaddr_un addr;

    addr.sun_family= AF_UNIX;
    size_t count= path.copy(addr.sun_path, sizeof(addr.sun_path)-1);
    addr.sun_path[count]=0;

    return addr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
sockaddr_in socket::from(net::address address, net::port port)
{
    FUNCTION_CONTEXT(ctx);
    sockaddr_in addr;

    addr.sin_family= AF_INET;
    addr.sin_addr.s_addr= htonl(address.value());
    addr.sin_port= htons(port);

    return addr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::bind(sockaddr* addr, socklen_t addr_len)
{
    FUNCTION_CONTEXT(ctx);
    if(::bind(_M_fd, addr, addr_len)) throw system_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::bind(const std::string& path)
{
    FUNCTION_CONTEXT(ctx);
    sockaddr_un addr(from(path));
    bind((sockaddr*)&addr, sizeof(addr));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::bind(net::address address, net::port port)
{
    FUNCTION_CONTEXT(ctx);
    sockaddr_in addr(from(address, port));
    bind((sockaddr*)&addr, sizeof(addr));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::connect(sockaddr* addr, socklen_t addr_len)
{
    FUNCTION_CONTEXT(ctx);
    if(::connect(_M_fd, addr, addr_len)) throw system_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::connect(const std::string& path)
{
    FUNCTION_CONTEXT(ctx);
    sockaddr_un addr(from(path));
    connect((sockaddr*)&addr, sizeof(addr));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::connect(net::address address, net::port port)
{
    FUNCTION_CONTEXT(ctx);
    sockaddr_in addr(from(address, port));
    connect((sockaddr*)&addr, sizeof(addr));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::listen(int max)
{
    FUNCTION_CONTEXT(ctx);
    if(::listen(_M_fd, max)) throw system_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::accept(net::socket& socket)
{
    FUNCTION_CONTEXT(ctx);
    socket._M_fd= ::accept(_M_fd, 0, 0);
    if(socket._M_fd <= 0) throw system_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::set_non_blocking(bool value)
{
    FUNCTION_CONTEXT(ctx);

    int opt= fcntl(_M_fd, F_GETFL);
    if(opt < 0) throw system_error();

    if(value)
        opt|= O_NONBLOCK;
    else
        opt&= ~O_NONBLOCK;

    if(fcntl(_M_fd, F_SETFL, opt)) throw system_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool socket::can_recv(timeval* tv)
{
    FUNCTION_CONTEXT(ctx);

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(_M_fd, &fds);

    int cnt= select(_M_fd+1, &fds, 0, 0, tv);
    if(cnt == -1) throw system_error();

    return cnt;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::set_multicast_loop(bool value)
{
    FUNCTION_CONTEXT(ctx);

    unsigned char opt= (value? 1: 0);
    if(setsockopt(_M_fd,
        IPPROTO_IP,
        IP_MULTICAST_LOOP,
        &opt,
        sizeof(opt)))
    throw system_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::set_multicast_ttl(unsigned char value)
{
    FUNCTION_CONTEXT(ctx);

    if(setsockopt(_M_fd,
        IPPROTO_IP,
        IP_MULTICAST_LOOP,
        &value,
        sizeof(value)))
    throw system_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::add_membership(net::address group)
{
    FUNCTION_CONTEXT(ctx);

    ip_mreq imr;
    imr.imr_multiaddr.s_addr= htonl(group.value());
    imr.imr_interface.s_addr= htonl(INADDR_ANY);

    if(setsockopt(_M_fd,
        IPPROTO_IP,
        IP_ADD_MEMBERSHIP,
        &imr,
        sizeof(imr)))
    throw system_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::drop_membership(net::address group)
{
    FUNCTION_CONTEXT(ctx);

    ip_mreq imr;
    imr.imr_multiaddr.s_addr= htonl(group.value());
    imr.imr_interface.s_addr= htonl(INADDR_ANY);

    if(setsockopt(_M_fd,
        IPPROTO_IP,
        IP_DROP_MEMBERSHIP,
        &imr,
        sizeof(imr)))
    throw system_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t socket::send(const void* buffer, size_t n, bool wait)
{
    FUNCTION_CONTEXT(ctx);

    ssize_t count= ::send(_M_fd, buffer, n, wait? 0: MSG_DONTWAIT);
    if(count == -1)
    {
        if(!wait && (errno==EAGAIN || errno==EWOULDBLOCK))
            return 0;
        else throw system_error();
    }
    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t socket::sendto(sockaddr* addr, socklen_t addr_len, const void* buffer, size_t n, bool wait)
{
    FUNCTION_CONTEXT(ctx);

    ssize_t count= ::sendto(_M_fd, buffer, n, wait? 0: MSG_DONTWAIT, addr, addr_len);
    if(count == -1)
    {
        if(!wait && (errno==EAGAIN || errno==EWOULDBLOCK))
            return 0;
        else throw system_error();
    }
    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t socket::sendto(const std::string& path, const void* buffer, size_t n, bool wait)
{
    FUNCTION_CONTEXT(ctx);
    sockaddr_un addr(from(path));
    return sendto((sockaddr*)&addr, sizeof(addr), buffer, n, wait);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t socket::sendto(net::address address, net::port port, const void* buffer, size_t n, bool wait)
{
    FUNCTION_CONTEXT(ctx);
    sockaddr_in addr(from(address, port));
    return sendto((sockaddr*)&addr, sizeof(addr), buffer, n, wait);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t socket::recv(std::string& string, size_t max, bool wait)
{
    FUNCTION_CONTEXT(ctx);
    std::unique_ptr<char[]> buffer(new char[max+1]);

    ssize_t count= recv(buffer.get(), max, wait);
    buffer[count]=0;

    string.assign(buffer.get(), count);
    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t socket::recv(void* buffer, size_t max, bool wait)
{
    FUNCTION_CONTEXT(ctx);

    ssize_t count= ::recv(_M_fd, buffer, max, wait? 0: MSG_DONTWAIT);
    if(count == -1)
    {
        if(!wait && (errno==EAGAIN || errno==EWOULDBLOCK))
            return 0;
        else throw system_error();
    }
    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t socket::recvfrom(sockaddr* addr, socklen_t& addr_len, void* buffer, size_t n, bool wait)
{
    FUNCTION_CONTEXT(ctx);

    ssize_t count= ::recvfrom(_M_fd, buffer, n, wait? 0: MSG_DONTWAIT, addr, &addr_len);
    if(count == -1)
    {
        if(!wait && (errno==EAGAIN || errno==EWOULDBLOCK))
            return 0;
        else throw system_error();
    }
    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t socket::recvfrom(std::string& path, std::string& string, size_t max, bool wait)
{
    FUNCTION_CONTEXT(ctx);
    std::unique_ptr<char[]> buffer(new char[max+1]);

    ssize_t count= recvfrom(path, buffer.get(), max, wait);
    buffer[count]=0;

    string.assign(buffer.get(), count);
    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t socket::recvfrom(std::string& path, void* buffer, size_t n, bool wait)
{
    FUNCTION_CONTEXT(ctx);

    sockaddr_un addr;
    socklen_t addr_len= sizeof(addr);
    memset(&addr, 0, addr_len);

    ssize_t count= recvfrom((sockaddr*)&addr, addr_len, buffer, n, wait);

    if(addr.sun_family==AF_UNIX) path.assign(addr.sun_path);

    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t socket::recvfrom(net::address& address, net::port& port, std::string& string, size_t max, bool wait)
{
    FUNCTION_CONTEXT(ctx);
    std::unique_ptr<char[]> buffer(new char[max+1]);

    ssize_t count= recvfrom(address, port, buffer.get(), max, wait);
    buffer[count]=0;

    string.assign(buffer.get(), count);
    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t socket::recvfrom(net::address& address, net::port& port, void* buffer, size_t n, bool wait)
{
    FUNCTION_CONTEXT(ctx);

    sockaddr_in addr;
    socklen_t addr_len= sizeof(addr);
    memset(&addr, 0, addr_len);

    ssize_t count= recvfrom((sockaddr*)&addr, addr_len, buffer, n, wait);

    if(addr.sin_family==AF_INET)
    {
        address.set_value(ntohl(addr.sin_addr.s_addr));
        port= ntohs(addr.sin_port);
    }

    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}
