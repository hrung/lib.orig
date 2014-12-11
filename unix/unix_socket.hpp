///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef UNIX_SOCKET_HPP
#define UNIX_SOCKET_HPP

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "socket/socket.hpp"

#include <string>
#include <sys/un.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
#undef unix

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace unix
{

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

    socket(socket::type);

    socket& operator=(const socket&) = delete;
    socket& operator=(socket&& x) noexcept
    {
        swap(x);
        return (*this);
    }

    void bind(const std::string& path);
    void connect(const std::string& path);

    size_t send_to(const std::string& path, const std::string& string, bool wait = true)
        { return send_to(path, string.data(), string.size(), wait); }
    size_t send_to(const std::string& path, const void* buffer, size_t n, bool wait = true);

    size_t recv_from(std::string& path, std::string& string, size_t max, bool wait = true);
    size_t recv_from(std::string& path, void* buffer, size_t n, bool wait = true);

private:
    sockaddr_un from(const std::string&);
    using base = app::socket;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif
