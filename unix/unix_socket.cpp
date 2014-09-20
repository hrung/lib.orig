///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "unix_socket.h"
#include "errno_error.h"

#include <memory>
#include <cstring>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace unix
{

///////////////////////////////////////////////////////////////////////////////////////////////////
socket::socket(type x): app::socket(AF_UNIX, x) { }

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
void socket::bind(const std::string& path)
{
    sockaddr_un addr= from(path);
    base::bind((sockaddr*)&addr, sizeof(addr));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void socket::connect(const std::string& path)
{
    sockaddr_un addr= from(path);
    base::connect((sockaddr*)&addr, sizeof(addr));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t socket::send_to(const std::string& path, const void* buffer, size_t n, bool wait)
{
    sockaddr_un addr(from(path));
    return base::send_to((sockaddr*)&addr, sizeof(addr), buffer, n, wait);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t socket::recv_from(std::string& path, std::string& string, size_t max, bool wait)
{
    std::unique_ptr<char[]> buffer(new char[max+1]);

    ssize_t count= recv_from(path, buffer.get(), max, wait);
    buffer[count]=0;

    string.assign(buffer.get(), count);
    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ssize_t socket::recv_from(std::string& path, void* buffer, size_t n, bool wait)
{
    sockaddr_un addr;
    socklen_t addr_len= sizeof(addr);
    memset(&addr, 0, addr_len);

    ssize_t count= base::recv_from((sockaddr*)&addr, addr_len, buffer, n, wait);

    if(addr.sun_family == AF_UNIX) path.assign(addr.sun_path);

    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}
