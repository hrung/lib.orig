///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef SLAP_H
#define SLAP_H

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "slap_type.h"
#include "optional.h"
#include "convert.hpp"
#include "container.h"

#include <functional>
#include <initializer_list>
#include <type_traits>
#include <utility>
#include <vector>
#include <set>
#include <string>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace slap
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
enum class operation
{
    add,
    remove,
    replace
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class attribute: public container<std::vector<std::string>>
{
public:
    typedef container_type values;

public:
    ////////////////////
    explicit attribute(const std::string& name, slap::operation mod= operation::add):
        _M_name(name),
        _M_operation(mod)
    { }
    explicit attribute(std::string&& name, slap::operation mod= operation::add):
        _M_name(std::move(name)),
        _M_operation(mod)
    { }

    ////////////////////
    attribute(const std::string& name, std::initializer_list<value_type> values):
        _M_name(name),
        _M_operation(operation::add)
    { insert(values); }

    attribute(std::string&& name, std::initializer_list<value_type> values):
        _M_name(std::move(name)),
        _M_operation(operation::add)
    { insert(values); }

    ////////////////////
    attribute(const std::string& name, slap::operation mod, std::initializer_list<value_type> values):
        _M_name(name),
        _M_operation(mod)
    { insert(values); }

    attribute(std::string&& name, slap::operation mod, std::initializer_list<value_type> values):
        _M_name(std::move(name)),
        _M_operation(mod)
    { insert(values); }

    ////////////////////
    template<typename T>
    attribute(const std::string& name, T&& value):
        _M_name(name),
        _M_operation(operation::add)
    { insert(std::forward<T>(value)); }

    template<typename T>
    attribute(std::string&& name, T&& value):
        _M_name(std::move(name)),
        _M_operation(operation::add)
    { insert(std::forward<T>(value)); }

    ////////////////////
    template<typename T>
    attribute(const std::string& name, slap::operation mod, T&& value):
        _M_name(name),
        _M_operation(mod)
    { insert(std::forward<T>(value)); }

    template<typename T>
    attribute(std::string&& name, slap::operation mod, T&& value):
        _M_name(std::move(name)),
        _M_operation(mod)
    { insert(std::forward<T>(value)); }

    ////////////////////
    attribute(const attribute&) = default;
    attribute(attribute&&) = default;

    attribute& operator=(const attribute&) = default;
    attribute& operator=(attribute&&) = default;

    ////////////////////
    const std::string& name() const noexcept { return _M_name; }
    slap::operation operation() const noexcept { return _M_operation; }

    ////////////////////
    reference value(size_type n=0) { return _M_c.at(n); }
    const_reference value(size_type n=0) const { return _M_c.at(n); }

    reference operator[](size_type n) { return _M_c[n]; }
    const_reference operator[](size_type n) const { return _M_c[n]; }

    ////////////////////
    template<typename ToType, typename std::enable_if< !std::is_same<ToType, bool>::value, int >::type=0>
    ToType to(size_type n=0) const
    {
        return convert::to<ToType>(value(n));
    }
    template<typename ToType, typename std::enable_if< std::is_same<ToType, bool>::value, int >::type=0>
    ToType to(size_type n=0) const
    {
        return (value(n)=="TRUE")? true: false;
    }

    template<typename ToType>
    void get(ToType& value, size_type n=0) const noexcept
    try
    {
        value= to<ToType>(n);
    }
    catch(std::invalid_argument&)
    {
        /* do nothing */
    }

    ////////////////////
    using container::insert;
    void insert(bool value) { insert(value? "TRUE": "FALSE"); }

    template<typename T>
    void insert(const T& value) { insert(convert::to<value_type>(value)); }

    using container::erase;
    iterator erase(const value_type& value) { return _M_c.erase(find(value)); }

    ////////////////////
    iterator find(const value_type& value, iterator from)
    {
        for(iterator ri= from; ri != end(); ++ri) if(value == *ri) return ri;
        return end();
    }
    iterator find(const value_type& value) { return find(value, begin()); }

    const_iterator find(const value_type& value, const_iterator from) const
    {
        for(const_iterator ri= from; ri!=end(); ++ri) if(value == *ri) return ri;
        return end();
    }
    const_iterator find(const value_type& value) const { return find(value, begin()); }

private:
    std::string _M_name;
    slap::operation _M_operation;

    slap::mod get_mod() const { return to_mod(static_cast<int>(_M_operation), _M_name, _M_c); }
    friend class entry;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
inline bool operator<(const attribute& x, const attribute& y) { return x.name() < y.name(); }

///////////////////////////////////////////////////////////////////////////////////////////////////
class entry: public container<std::set<slap::attribute>>
{
public:
    typedef container_type attributes;

public:
    ////////////////////
    explicit entry(const std::string& dn): _M_dn(dn) { }
    explicit entry(std::string&& dn): _M_dn(std::move(dn)) { }

    entry(const std::string& dn, std::initializer_list<value_type> attributes):
        _M_dn(dn)
    { insert(attributes); }

    entry(std::string&& dn, std::initializer_list<value_type> attributes):
        _M_dn(std::move(dn))
    { insert(attributes); }

    ////////////////////
    entry(const entry&) = default;
    entry(entry&&) = default;

    entry& operator=(const entry&) = default;
    entry& operator=(entry&&) = default;

    ////////////////////
    const std::string& dn() const noexcept { return _M_dn; }

    ////////////////////
    reference attribute(const std::string& name)
    {
        iterator ri= find(name);
        if(ri == end()) throw std::out_of_range("entry::attribute()");
        return const_cast<reference>(*ri); // o.O
    }
    const_reference attribute(const std::string& name) const
    {
        const_iterator ri= find(name);
        if(ri == cend()) throw std::out_of_range("entry::attribute()");
        return *ri;
    }

    reference operator[](const std::string& name) { return attribute(name); }
    const_reference operator[](const std::string& name) const { return attribute(name); }

    value_type::reference attribute_value(const std::string& name, value_type::size_type n=0)
    {
        return attribute(name).value(n);
    }
    value_type::const_reference attribute_value(const std::string& name, value_type::size_type n=0) const
    {
        return attribute(name).value(n);
    }

    ////////////////////
    template<typename ToType>
    ToType attribute_to(const std::string& name, value_type::size_type n=0) const
    {
        return convert<ToType>::from(*this, name, n);
    }

    template<typename ToType>
    void attribute_get(ToType& value, const std::string& name, value_type::size_type n=0) const noexcept
    try
    {
        value= attribute_to<ToType>(name, n);
    }
    catch(std::invalid_argument&)
    {
        /* do nothing*/
    }
    catch(std::out_of_range&)
    {
        /* do nothing*/
    }

    ////////////////////
    using container::insert;
    using container::erase;

    size_type erase(const value_type& value) { return _M_c.erase(value); }
    size_type erase(const std::string& name) { return _M_c.erase(value_type(name)); }

    ////////////////////
    iterator find(const value_type& value) { return _M_c.find(value); }
    const_iterator find(const value_type& value) const { return _M_c.find(value); }

    iterator find(const std::string& name) { return _M_c.find(value_type(name)); }
    const_iterator find(const std::string& name) const { return _M_c.find(value_type(name)); }

    size_type count(const value_type& value) const { return _M_c.count(value); }
    size_type count(const std::string& name) const { return _M_c.count(value_type(name)); }

private:
    std::string _M_dn;

    std::vector<slap::mod> get_mod() const;
    friend class connection;

    ////////////////////
    template<typename ToType>
    struct convert
    {
        static ToType from(const slap::entry& e, const std::string& name, typename value_type::size_type n=0)
        {
            return e.attribute(name).to<ToType>(n);
        }
    };

    template<typename ToType>
    struct convert<optional<ToType>>
    {
        static optional<ToType> from(const slap::entry& e, const std::string& name, typename value_type::size_type n=0)
        {
            const_iterator ri= e.find(name);
            if(ri != e.cend())
                return ri->to<ToType>(n);
            else return optional<ToType>();
        }
    };
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef std::vector<entry> entries;

typedef std::set<std::string> names;
typedef std::function<bool(const entry&, const entry&)> order_func;

const extern order_func order_none;

enum class scope
{
    base,
    one_level,
    subtree,
    children
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class connection
{
public:
    connection() = default;
    connection(const connection&) = delete;

    connection(connection&& x) noexcept { swap(x); }

    connection(const std::string& uri, bool start_tls= true);
    ~connection() { close(); }

    void close() noexcept;
    bool is_open() const noexcept { return _M_ldap != nullptr; }

    connection& operator=(const connection&) = delete;
    connection& operator=(connection&& x) noexcept
    {
        swap(x);
        return (*this);
    }
    void swap(connection& x) noexcept { std::swap(_M_ldap, x._M_ldap); }

    ////////////////////
    void bind(const std::string& dn, const std::string& passwd= std::string());

    void add(const entry& e);
    void remove(const std::string& dn);
    void modify(const entry& e);

    void rename(const std::string& dn,
                const std::string& new_rdn,
                bool remove_old= true,
    const std::string& new_parent= std::string());

    bool compare(const std::string& dn, const attribute&);

    entries search(const std::string& base,
                   scope = scope::base,
                   const std::string& filter= "objectClass=*",
                   const order_func& func= order_none,
                   const names& = names(),
    bool get_value= true);

private:
    ldap _M_ldap= nullptr;

    modpp get_mod(std::vector<mod>&);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // SLAP_H
