///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef SOCKET_H
#define SOCKET_H

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "except.h"
#include <string>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace net
{

///////////////////////////////////////////////////////////////////////////////////////////////////
struct ip_address
{
    ip_address() { }
    ip_address(in_addr_t x): value(x) { }

    ip_address(const std::string& address)
    {
        if(address.empty()) return;

        in_addr ina;
        if(!inet_aton(address.data(), &ina))
            throw system_except();
        else value= ntohl(ina.s_addr);
    }
    ip_address(const char* address): ip_address(std::string(address)) { }

    operator std::string() {  return inet_ntoa({ htonl(value) }); }

    in_addr_t value= INADDR_ANY;
};

typedef uint16_t port;

typedef int desc;
const desc invalid_desc= -1;

///////////////////////////////////////////////////////////////////////////////////////////////////
enum class type
{
    stream= SOCK_STREAM,
    datagram= SOCK_DGRAM,
    raw= SOCK_RAW
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class socket
{
public:
    socket() { }
    socket(type _m_type) { create(_m_type); }

    socket(const socket&) = delete;
    socket& operator=(const socket&) = delete;

    socket(socket&&) = default;
    socket& operator=(socket&&) = default;

    virtual ~socket() { close(); }

    void create(type);
    void close();
    bool is_created() const { return _M_fd != invalid_desc; }

    void bind(const std::string& ip_local, port p) { bind(ip_address(ip_local), p); }
    void bind(ip_address ip_local, port p);
    void bind(port p) { bind(ip_address(INADDR_ANY), p); }

    void connect(const std::string& ip_remote, port p) { connect(ip_address(ip_remote), p); }
    void connect(ip_address ip_remote, port p);

    void listen(int max= 128);
    void accept(socket& accept_socket);

    void set_non_blocking(bool);
    bool can_recv(int wait_usec=0) { timeval tv={0, wait_usec}; return can_recv((wait_usec<0)? 0: &tv); }
    bool can_recv(timeval* tv);

    void set_multicast_loop(bool);
    void set_multicast_ttl(unsigned char);

    void add_membership(const std::string& ip_group) { add_membership(ip_address(ip_group)); }
    void add_membership(ip_address ip_group);

    void drop_membership(const std::string& ip_group) { drop_membership(ip_address(ip_group)); }
    void drop_membership(ip_address ip_group);

    ssize_t send(const std::string& s, bool wait= true)
        { return send(s.data(), s.size(), wait); }
    ssize_t send(const void* b, size_t n, bool wait= true);

    ssize_t sendto(const std::string& ip_remote, port p, const std::string& s, bool wait= true)
        { return sendto(ip_address(ip_remote), p, s.data(), s.size(), wait); }
    ssize_t sendto(ip_address ip_remote, port p, const std::string& s, bool wait= true)
        { return sendto(ip_remote, p, s.data(), s.size(), wait); }

    ssize_t sendto(const std::string& ip_remote, port p, const void* b, size_t n, bool wait= true)
        { return sendto(ip_address(ip_remote), p, b, n, wait); }
    ssize_t sendto(ip_address ip_remote, port p, const void* b, size_t n, bool wait= true);

    ssize_t recv(std::string& s, size_t max, bool wait= true);
    ssize_t recv(void* b, size_t max, bool wait= true);

    ssize_t recvfrom(std::string& ip_remote, port& p, std::string& s, size_t max, bool wait= true)
    {
        ip_address address;
        ssize_t n= recvfrom(address, p, s, max, wait);

        ip_remote= address;
        return n;
    }
    ssize_t recvfrom(ip_address& ip_remote, port& p, std::string& s, size_t max, bool wait= true);

    ssize_t recvfrom(std::string& ip_remote, port& p, void* b, size_t max, bool wait= true)
    {
        ip_address address;
        ssize_t n= recvfrom(address, p, b, max, wait);

        ip_remote= address;
        return n;
    }
    ssize_t recvfrom(ip_address& ip_remote, port& p, void* b, size_t max, bool wait= true);

    desc fd() const { return _M_fd; }

protected:
    desc _M_fd= invalid_desc;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif
