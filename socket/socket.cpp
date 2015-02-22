///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "errno_error.hpp"
#include "socket.hpp"

#include <ctime>
#include <memory>

#include <fcntl.h>
#include <sys/select.h>
#include <unistd.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace app
{

///////////////////////////////////////////////////////////////////////////////////////////////////
socket::socket(int family, socket::type type)
{
    int val;
    switch(type)
    {
    case socket::datagram:
        val = SOCK_DGRAM;
        break;
    case socket::stream:
        val = SOCK_STREAM;
        break;
    default:
        throw std::runtime_error("socket::socket(): unsupported type");
    }

    _M_fd = ::socket(family, val, 0);
    if(_M_fd == invalid) throw errno_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::close() noexcept
{
    if(_M_fd != invalid)
    {
        ::close(_M_fd);
        _M_fd = invalid;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::bind(sockaddr* addr, socklen_t addr_len)
{
    if(::bind(_M_fd, addr, addr_len)) throw errno_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::connect(sockaddr* addr, socklen_t addr_len)
{
    if(::connect(_M_fd, addr, addr_len)) throw errno_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::listen(int max)
{
    if(::listen(_M_fd, max)) throw errno_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::accept(app::socket& socket)
{
    socket._M_fd = ::accept(_M_fd, nullptr, nullptr);
    if(socket._M_fd == invalid) throw errno_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::set_non_blocking(bool value)
{
    int opt = fcntl(_M_fd, F_GETFL);
    if(opt < 0) throw errno_error();

    if(value)
        opt |= O_NONBLOCK;
    else
        opt &= ~O_NONBLOCK;

    if(fcntl(_M_fd, F_SETFL, opt)) throw errno_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool socket::can_recv(std::chrono::seconds s, std::chrono::nanoseconds n)
{
    timespec time = { static_cast<std::time_t>(s.count()), static_cast<long>(n.count()) };

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(_M_fd, &fds);

    int count = pselect(_M_fd+1, &fds, nullptr, nullptr, &time, nullptr);
    if(count == -1) throw errno_error();

    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
size_t socket::send(const void* buffer, size_t n, bool wait)
{
    ssize_t count = ::send(_M_fd, buffer, n, (wait ? 0 : MSG_DONTWAIT) | MSG_NOSIGNAL);
    if(count == -1)
    {
        if(!wait && (errno == EAGAIN || errno == EWOULDBLOCK))
            return 0;
        else throw errno_error();
    }
    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
size_t socket::send_to(sockaddr* addr, socklen_t addr_len, const void* buffer, size_t n, bool wait)
{
    ssize_t count = ::sendto(_M_fd, buffer, n, (wait ? 0 : MSG_DONTWAIT) | MSG_NOSIGNAL, addr, addr_len);
    if(count == -1)
    {
        if(!wait && (errno == EAGAIN || errno == EWOULDBLOCK))
            return 0;
        else throw errno_error();
    }
    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
size_t socket::recv(std::string& string, size_t max, bool wait)
{
    std::unique_ptr<char[]> buffer(new char[max + 1]);

    ssize_t count = recv(buffer.get(), max, wait);
    buffer[count] = '\0';

    string.assign(buffer.get(), count);
    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
size_t socket::recv(void* buffer, size_t max, bool wait)
{
    ssize_t count = ::recv(_M_fd, buffer, max, wait ? 0 : MSG_DONTWAIT);
    if(count == -1)
    {
        if(!wait && (errno == EAGAIN || errno == EWOULDBLOCK))
            return 0;
        else throw errno_error();
    }
    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
size_t socket::recv_from(sockaddr* addr, socklen_t& addr_len, void* buffer, size_t n, bool wait)
{
    ssize_t count = ::recvfrom(_M_fd, buffer, n, wait ? 0 : MSG_DONTWAIT, addr, &addr_len);
    if(count == -1)
    {
        if(!wait && (errno == EAGAIN || errno == EWOULDBLOCK))
            return 0;
        else throw errno_error();
    }
    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}
