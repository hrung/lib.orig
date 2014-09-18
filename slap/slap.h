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
#include "slap_error.h"
#include "optional.h"
#include "convert.h"
#include "container.h"

#include <functional>
#include <initializer_list>
#include <type_traits>
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
        _M_name(name), _M_operation(mod)
    { }
    explicit attribute(std::string&& name, slap::operation mod= operation::add):
        _M_name(std::move(name)), _M_operation(mod)
    { }

    ////////////////////
    attribute(const std::string& name, std::initializer_list<value_type> values):
        _M_name(name), _M_operation(operation::add)
    { _M_c= values; }

    attribute(std::string&& name, std::initializer_list<value_type> values):
        _M_name(std::move(name)), _M_operation(operation::add)
    { _M_c= values; }

    ////////////////////
    attribute(const std::string& name, slap::operation mod, std::initializer_list<value_type> values):
        _M_name(name), _M_operation(mod)
    { _M_c= values; }

    attribute(std::string&& name, slap::operation mod, std::initializer_list<value_type> values):
        _M_name(std::move(name)), _M_operation(mod)
    { _M_c= values; }

    ////////////////////
    template<typename T>
    attribute(const std::string& name, T&& value): _M_name(name), _M_operation(operation::add)
    {
        insert(std::forward<T>(value));
    }
    template<typename T>
    attribute(std::string&& name, T&& value): _M_name(std::move(name)), _M_operation(operation::add)
    {
        insert(std::forward<T>(value));
    }

    ////////////////////
    template<typename T>
    attribute(const std::string& name, slap::operation mod, T&& value): _M_name(name), _M_operation(mod)
    {
        insert(std::forward<T>(value));
    }
    template<typename T>
    attribute(std::string&& name, slap::operation mod, T&& value): _M_name(std::move(name)), _M_operation(mod)
    {
        insert(std::forward<T>(value));
    }

   ~attribute() { delete_mod(); }

    ////////////////////
    attribute(const attribute& x):
        _M_name(x._M_name),
        _M_operation(x._M_operation)
    { _M_c= x._M_c; }

    attribute(attribute&& x):
        _M_name(std::move(x._M_name)),
        _M_operation(std::move(x._M_operation)),
        _M_mod(std::move(x._M_mod))
    {
        _M_c= std::move(x._M_c);
        x._M_mod= nullptr;
    }

    ////////////////////
    attribute& operator=(const attribute& x)
    {
        _M_name= x._M_name;
        _M_operation= x._M_operation;
        _M_c= x._M_c;
        return *this;
    }
    attribute& operator=(attribute&& x)
    {
        _M_name= std::move(x._M_name);
        _M_operation= std::move(x._M_operation);
        _M_c= std::move(x._M_c);
        delete_mod();
        _M_mod= std::move(x._M_mod);
        x._M_mod= nullptr;
        return *this;
    }

    ////////////////////
    const std::string& name() const { return _M_name; }
    slap::operation operation() const { return _M_operation; }

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
    void insert(const value_type& value) { _M_c.push_back(value); }
    void insert(value_type&& value) { _M_c.push_back(std::move(value)); }
    void insert(std::initializer_list<value_type> values) { _M_c.insert(_M_c.end(), values); }

    void insert(bool value) { _M_c.push_back(value? "TRUE": "FALSE"); }

    template<typename T>
    void insert(const T& value) { insert(convert::to<value_type>(value)); }

    iterator erase(const value_type& value) { return _M_c.erase(find(value)); }

    iterator erase(const_iterator ri_0, const_iterator ri_1) { return _M_c.erase(ri_0, ri_1); }
    iterator erase(const_iterator ri) { return _M_c.erase(ri); }

    ////////////////////
    iterator find(const value_type& value)
    {
        for(auto ri= begin(); ri!=end(); ++ri) if(value == *ri) return ri;
        return end();
    }
    const_iterator find(const value_type& value) const
    {
        for(auto ri= begin(); ri!=end(); ++ri) if(value == *ri) return ri;
        return end();
    }

private:
    std::string     _M_name;
    slap::operation _M_operation;

    mutable LDAPMod* _M_mod= nullptr;

    void create_mod() const;
    void delete_mod() const;

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
    { _M_c= attributes; }

    entry(std::string&& dn, std::initializer_list<value_type> attributes):
        _M_dn(std::move(dn))
    { _M_c= attributes; }

   ~entry() { delete_mod(); }

    ////////////////////
    entry(const entry& x): _M_dn(x._M_dn) { _M_c= x._M_c; }

    entry(entry&& x):
        _M_dn(std::move(x._M_dn)),
        _M_mod(std::move(x._M_mod))
    {
        _M_c= std::move(x._M_c);
        x._M_mod= nullptr;
    }

    ////////////////////
    entry& operator=(const entry& x)
    {
        _M_dn= x._M_dn;
        _M_c= x._M_c;
        return *this;
    }
    entry& operator=(entry&& x)
    {
        _M_dn= std::move(x._M_dn);
        _M_c= std::move(x._M_c);
        delete_mod();
        _M_mod= std::move(x._M_mod);
        x._M_mod= nullptr;
        return *this;
    }

    ////////////////////
    const std::string& dn() const { return _M_dn; }

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
        return _M_to<ToType>::func(*this, name, n);
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

    ////////////////////
    std::pair<iterator,bool> insert(const value_type& x) { return _M_c.insert(x); }
    std::pair<iterator,bool> insert(value_type&& x) { return _M_c.insert(std::move(x)); }
    void insert(std::initializer_list<value_type> x) { _M_c.insert(x); }

    size_type erase(const value_type& value) { return _M_c.erase(value); }
    size_type erase(const std::string& name) { return _M_c.erase(slap::attribute(name)); }

    iterator erase(const_iterator ri_0, iterator ri_1) { return _M_c.erase(ri_0, ri_1); }
    iterator erase(iterator ri) { return _M_c.erase(ri); }

    ////////////////////
    iterator find(const value_type& value) { return _M_c.find(value); }
    const_iterator find(const value_type& value) const { return _M_c.find(value); }

    iterator find(const std::string& name) { return _M_c.find(value_type(name)); }
    const_iterator find(const std::string& name) const { return _M_c.find(value_type(name)); }

    size_type count(const value_type& value) const { return _M_c.count(value); }
    size_type count(const std::string& name) const { return _M_c.count(value_type(name)); }

private:
    std::string      _M_dn;
    mutable LDAPMod** _M_mod= nullptr;

    void create_mod() const;
    void delete_mod() const;

    friend class connection;

    ////////////////////
    template<typename ToType>
    struct _M_to
    {
        static ToType func(const slap::entry& e, const std::string& name, value_type::size_type n=0)
        {
            return e.attribute(name).to<ToType>(n);
        }
    };

    template<typename ToType>
    struct _M_to<optional<ToType>>
    {
        static optional<ToType> func(const slap::entry& e, const std::string& name, value_type::size_type n=0)
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
    connection() { }
    connection(const std::string& uri) { open(uri); }
    connection(std::string&& uri) { open(std::move(uri)); }

   ~connection() { close(); }

    ////////////////////
    connection(const connection&) = delete;
    connection& operator=(const connection&) = delete;

    connection(connection&&) = delete;
    connection& operator=(connection&&) = delete;

    ////////////////////
    void open(const std::string& uri, bool start_TLS= true)
    {
        _M_uri= uri;
        _M_start_TLS= start_TLS;
        _M_open();
    }
    void open(std::string&& uri, bool start_TLS= true)
    {
        _M_uri= std::move(uri);
        _M_start_TLS= start_TLS;
        _M_open();
    }
    void close();

    void bind(const std::string& dn, const std::string& passwd= std::string())
    {
        _M_bind_dn= dn;
        _M_passwd= passwd;
        _M_bind();
    }
    void bind(std::string&& dn, const std::string& passwd= std::string())
    {
        _M_bind_dn= std::move(dn);
        _M_passwd= passwd;
        _M_bind();
    }

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

    ////////////////////
    std::string uri() const { return _M_uri; }
    std::string bind_dn() const { return _M_bind_dn; }

private:
    std::string _M_uri;
    bool _M_start_TLS= false;

    std::string _M_bind_dn, _M_passwd;

    mutable LDAP* _M_ldap= nullptr;

    void _M_open();
    void _M_bind();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // SLAP_H
