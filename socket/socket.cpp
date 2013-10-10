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
void socket::create(type _m_type)
{
    _M_fd= ::socket(PF_INET, int(_m_type), 0);
    if(_M_fd == invalid_desc) throw system_except();

    int value=1;
    if(setsockopt(_M_fd,
        SOL_SOCKET,
        SO_REUSEADDR,
        &value,
        sizeof(value)))
    throw system_except();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::close()
{
    if(_M_fd != invalid_desc)
    {
        ::close(_M_fd);
        _M_fd= invalid_desc;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::bind(ip_address ip_local, port p)
{
    sockaddr_in addr;
    addr.sin_family= AF_INET;
    addr.sin_addr.s_addr= htonl(ip_local.value);
    addr.sin_port= htons(p);

    if(::bind(_M_fd, (sockaddr*)&addr, sizeof(addr))) throw system_except();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::connect(ip_address ip_remote, port p)
{
    sockaddr_in addr;
    addr.sin_family= AF_INET;
    addr.sin_addr.s_addr= htonl(ip_remote.value);
    addr.sin_port= htons(p);

    if(::connect(_M_fd, (sockaddr*)&addr, sizeof(addr))) throw system_except();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::listen(int max)
{
    if(::listen(_M_fd, max)) throw system_except();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::accept(socket& accept_socket)
{
    accept_socket._M_fd= ::accept(_M_fd, 0, 0);

    if(accept_socket._M_fd <= 0) throw system_except();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::set_non_blocking(bool value)
{
    int opt= fcntl(_M_fd, F_GETFL);
    if(opt < 0) throw system_except();

    if(value)
        opt|= O_NONBLOCK;
    else
        opt&= ~O_NONBLOCK;

    if(fcntl(_M_fd, F_SETFL, opt)) throw system_except();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool socket::can_recv(timeval* tv)
{
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(_M_fd, &fds);

    int cnt= select(_M_fd+1, &fds, 0, 0, tv);
    if(cnt == -1) throw system_except();

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
    throw system_except();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::set_multicast_ttl(unsigned char value)
{
    if(setsockopt(_M_fd,
        IPPROTO_IP,
        IP_MULTICAST_LOOP,
        &value,
        sizeof(value)))
    throw system_except();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::add_membership(ip_address ip_group)
{
    ip_mreq imr;
    imr.imr_multiaddr.s_addr= htonl(ip_group.value);
    imr.imr_interface.s_addr= htonl(INADDR_ANY);

    if(setsockopt(_M_fd,
        IPPROTO_IP,
        IP_ADD_MEMBERSHIP,
        &imr,
        sizeof(imr)))
    throw system_except();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::drop_membership(ip_address ip_group)
{
    ip_mreq imr;
    imr.imr_multiaddr.s_addr= htonl(ip_group.value);
    imr.imr_interface.s_addr= htonl(INADDR_ANY);

    if(setsockopt(_M_fd,
        IPPROTO_IP,
        IP_DROP_MEMBERSHIP,
        &imr,
        sizeof(imr)))
    throw system_except();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t socket::send(const void* b, size_t n, bool wait)
{
    ssize_t len= ::send(_M_fd, b, n, wait? 0: MSG_DONTWAIT);
    if(len == -1)
    {
        if(!wait && (errno==EAGAIN || errno==EWOULDBLOCK))
            return 0;
        else throw system_except();
    }
    return len;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t socket::sendto(ip_address ip_remote, port p, const void* b, size_t n, bool wait)
{
    sockaddr_in addr;

    addr.sin_family= AF_INET;
    addr.sin_addr.s_addr= htonl(ip_remote.value);
    addr.sin_port= htons(p);

    ssize_t len= ::sendto(_M_fd, b, n, wait? 0: MSG_DONTWAIT, (sockaddr*)&addr, sizeof(addr));
    if(len == -1)
    {
        if(!wait && (errno==EAGAIN || errno==EWOULDBLOCK))
            return 0;
        else throw system_except();
    }
    return len;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t socket::recv(std::string& s, size_t max, bool wait)
{
    std::unique_ptr<char[]> buffer(new char[max+1]);

    ssize_t len= recv(buffer.get(), max, wait);
    buffer[len]=0;

    s.assign(buffer.get(), len);
    return len;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t socket::recv(void* b, size_t max, bool wait)
{
    ssize_t len= ::recv(_M_fd, b, max, wait? 0: MSG_DONTWAIT);
    if(len == -1)
    {
        if(!wait && (errno==EAGAIN || errno==EWOULDBLOCK))
            return 0;
        else throw system_except();
    }
    return len;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t socket::recvfrom(ip_address& ip_remote, port& p, std::string& s, size_t max, bool wait)
{
    std::unique_ptr<char[]> buffer(new char[max+1]);

    ssize_t len= recvfrom(ip_remote, p, buffer.get(), max, wait);
    buffer[len]=0;

    s.assign(buffer.get(), len);
    return len;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t socket::recvfrom(ip_address& ip_remote, port& p, void* b, size_t n, bool wait)
{
    sockaddr_in addr;
    socklen_t addr_len= sizeof(addr);
    memset(&addr, 0, addr_len);

    ssize_t len= ::recvfrom(_M_fd, b, n, wait? 0: MSG_DONTWAIT, (sockaddr*)&addr, &addr_len);
    if(len == -1)
    {
        if(!wait && (errno==EAGAIN || errno==EWOULDBLOCK))
            return 0;
        else throw system_except();
    }
    ip_remote.value= ntohl(addr.sin_addr.s_addr);
    p= ntohs(addr.sin_port);

    return len;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}
