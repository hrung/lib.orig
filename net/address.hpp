///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ADDRESS_HPP
#define ADDRESS_HPP

///////////////////////////////////////////////////////////////////////////////////////////////////
#include <string>
#include <netinet/in.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace net
{

///////////////////////////////////////////////////////////////////////////////////////////////////
class socket;

///////////////////////////////////////////////////////////////////////////////////////////////////
class address
{
public:
    static const address any;
    static const address broadcast;
    static const address loopback;

public:
    address() noexcept = default;
    address(const address&) noexcept = default;
    address(address&&) noexcept = default;

    address& operator=(const address&) noexcept = default;
    address& operator=(address&&) noexcept = default;

    ////////////////////
    ///
    /// \brief address constructor
    /// \param x in host byte order (!)
    ///
    address(in_addr x) noexcept: address(x.s_addr) { }

    ////////////////////
    ///
    /// \brief address constructor
    /// \param x in host byte order (!)
    ///
    address(in_addr_t x) noexcept;

    address(const std::string&);
    address(const char* x): address(std::string(x)) { }

    std::string to_string() const;

    ////////////////////
    ///
    /// \brief value
    /// \return in host byte order (!)
    ///
    in_addr_t value() const;

private:
    in_addr _M_addr; // in network byte order (big endian)
    friend class net::socket;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // ADDRESS_HPP
