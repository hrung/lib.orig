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

typedef std::vector<std::string> values;

///////////////////////////////////////////////////////////////////////////////////////////////////
class attribute
{
public:
    typedef values container_type;
    typedef typename container_type::value_type value_type;

    typedef typename container_type::reference reference;
    typedef typename container_type::const_reference const_reference;
    typedef typename container_type::pointer pointer;
    typedef typename container_type::const_pointer const_pointer;

    typedef typename container_type::size_type size_type;
    typedef typename container_type::iterator iterator;
    typedef typename container_type::const_iterator const_iterator;
    typedef typename container_type::reverse_iterator reverse_iterator;
    typedef typename container_type::const_reverse_iterator const_reverse_iterator;

    ////////////////////
    explicit attribute(const std::string& name, slap::operation mod= operation::add):
        _M_name(name), _M_operation(mod)
    { }
    explicit attribute(std::string&& name, slap::operation mod= operation::add):
        _M_name(std::move(name)), _M_operation(mod)
    { }

    ////////////////////
    attribute(const std::string& name, std::initializer_list<value_type> values):
        _M_name(name), _M_operation(operation::add), _M_values(values)
    { }
    attribute(std::string&& name, std::initializer_list<value_type> values):
        _M_name(std::move(name)), _M_operation(operation::add), _M_values(values)
    { }

    ////////////////////
    attribute(const std::string& name, slap::operation mod, std::initializer_list<value_type> values):
        _M_name(name), _M_operation(mod), _M_values(values)
    { }
    attribute(std::string&& name, slap::operation mod, std::initializer_list<value_type> values):
        _M_name(std::move(name)), _M_operation(mod), _M_values(values)
    { }

    ////////////////////
    template<typename T>
    attribute(const std::string& name, T&& value): _M_name(name), _M_operation(operation::add)
    {
        FUNCTION_CONTEXT(ctx);
        append(std::forward<T>(value));
    }
    template<typename T>
    attribute(std::string&& name, T&& value): _M_name(std::move(name)), _M_operation(operation::add)
    {
        FUNCTION_CONTEXT(ctx);
        append(std::forward<T>(value));
    }

    ////////////////////
    template<typename T>
    attribute(const std::string& name, slap::operation mod, T&& value): _M_name(name), _M_operation(mod)
    {
        FUNCTION_CONTEXT(ctx);
        append(std::forward<T>(value));
    }
    template<typename T>
    attribute(std::string&& name, slap::operation mod, T&& value): _M_name(std::move(name)), _M_operation(mod)
    {
        FUNCTION_CONTEXT(ctx);
        append(std::forward<T>(value));
    }

   ~attribute() { delete_mod(); }

    ////////////////////
    attribute(const attribute& other):
        _M_name      (other._M_name),
        _M_operation (other._M_operation),
        _M_values    (other._M_values)
    { }
    attribute& operator=(const attribute& other)
    {
        _M_name=      other._M_name;
        _M_operation= other._M_operation;
        _M_values=    other._M_values;
        return *this;
    }

    ////////////////////
    attribute(attribute&& other):
        _M_name      (std::move(other._M_name)),
        _M_operation (std::move(other._M_operation)),
        _M_values    (std::move(other._M_values)),
        _M_mod       (std::move(other._M_mod))
    { other._M_mod= nullptr; }

    attribute& operator=(attribute&& other)
    {
        _M_name=      std::move(other._M_name);
        _M_operation= std::move(other._M_operation);
        _M_values=    std::move(other._M_values);
        delete_mod();
        _M_mod=       std::move(other._M_mod);
        other._M_mod= nullptr;
        return *this;
    }

    ////////////////////
    const std::string& name() const { return _M_name; }
    slap::operation operation() const { return _M_operation; }

    ////////////////////
    bool empty() const { return _M_values.empty(); }
    size_type size() const { return _M_values.size(); }
    void clear() { _M_values.clear(); }

    ////////////////////
    reference operator[](size_type n)
    {
        FUNCTION_CONTEXT(ctx);
        try { return _M_values.at(n); }
        catch(std::out_of_range& e) { throw out_of_range(e); }
    }
    const_reference operator[](size_type n) const
    {
        FUNCTION_CONTEXT(ctx);
        try { return _M_values.at(n); }
        catch(std::out_of_range& e) { throw out_of_range(e); }
    }

    ////////////////////
    template<typename T, typename std::enable_if< !std::is_same<T, bool>::value, int >::type=0>
    T to(size_type n=0) const
    {
        FUNCTION_CONTEXT(ctx);
        return convert::to<T>(operator[](n));
    }
    template<typename T, typename std::enable_if< std::is_same<T, bool>::value, int >::type=0>
    T to(size_type n=0) const
    {
        FUNCTION_CONTEXT(ctx);
        return (operator[](n)=="TRUE")? true: false;
    }

    ////////////////////
    void append(const value_type& value) { _M_values.push_back(value); }
    void append(value_type&& value) { _M_values.push_back(std::move(value)); }
    void append(std::initializer_list<value_type> values) { _M_values.insert(_M_values.end(), values); }

    void append(bool value) { _M_values.push_back(value? "TRUE": "FALSE"); }

    template<typename T>
    void append(const T& value)
    {
        FUNCTION_CONTEXT(ctx);
        append(convert::to<value_type>(value));
    }

    iterator remove(const value_type& value) { return _M_values.erase(find(value)); }

    iterator remove(const_iterator ri_0, const_iterator ri_1) { return _M_values.erase(ri_0, ri_1); }
    iterator remove(const_iterator ri) { return _M_values.erase(ri); }

    ////////////////////
    iterator begin() { return _M_values.begin(); }
    const_iterator begin() const { return _M_values.begin(); }

    iterator end() { return _M_values.end(); }
    const_iterator end() const { return _M_values.end(); }

    reverse_iterator rbegin() { return _M_values.rbegin(); }
    const_reverse_iterator rbegin() const { return _M_values.rbegin(); }

    reverse_iterator rend() { return _M_values.rend(); }
    const_reverse_iterator rend() const { return _M_values.rend(); }

    const_iterator cbegin() const { return _M_values.cbegin(); }
    const_iterator cend() const { return _M_values.cend(); }

    const_reverse_iterator crbegin() const { return _M_values.crbegin(); }
    const_reverse_iterator crend() const { return _M_values.crend(); }

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
    slap::values    _M_values;

    mutable LDAPMod* _M_mod= nullptr;

    void create_mod() const;
    void delete_mod() const;

    friend class entry;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
inline bool operator<(const attribute& x, const attribute& y) { return x.name() < y.name(); }

typedef std::set<attribute> attributes;

///////////////////////////////////////////////////////////////////////////////////////////////////
class entry
{
public:
    typedef attributes container_type;
    typedef typename container_type::value_type value_type;

    typedef typename container_type::reference reference;
    typedef typename container_type::const_reference const_reference;
    typedef typename container_type::pointer pointer;
    typedef typename container_type::const_pointer const_pointer;

    typedef typename container_type::size_type size_type;
    typedef typename container_type::iterator iterator;
    typedef typename container_type::const_iterator const_iterator;
    typedef typename container_type::reverse_iterator reverse_iterator;
    typedef typename container_type::const_reverse_iterator const_reverse_iterator;

    ////////////////////
    explicit entry(const std::string& dn): _M_dn(dn) { }
    explicit entry(std::string&& dn): _M_dn(std::move(dn)) { }

    entry(const std::string& dn, std::initializer_list<attribute> attributes):
        _M_dn(dn), _M_attributes(attributes)
    { }
    entry(std::string&& dn, std::initializer_list<attribute> attributes):
        _M_dn(std::move(dn)), _M_attributes(attributes)
    { }

   ~entry() { delete_mod(); }

    ////////////////////
    entry(const entry& other):
        _M_dn         (other._M_dn),
        _M_attributes (other._M_attributes)
    { }
    entry& operator=(const entry& other)
    {
        _M_dn=         other._M_dn;
        _M_attributes= other._M_attributes;
        return *this;
    }

    ////////////////////
    entry(entry&& other):
        _M_dn         (std::move(other._M_dn)),
        _M_attributes (std::move(other._M_attributes)),
        _M_mod        (std::move(other._M_mod))
    { other._M_mod= nullptr; }

    entry& operator=(entry&& other)
    {
        _M_dn=         std::move(other._M_dn);
        _M_attributes= std::move(other._M_attributes);
        delete_mod();
        _M_mod=        std::move(other._M_mod);
        other._M_mod= nullptr;
        return *this;
    }

    ////////////////////
    std::string dn() const { return _M_dn; }

    bool empty() const { return _M_attributes.empty(); }
    size_type size() const { return _M_attributes.size(); }
    void clear() { _M_attributes.clear(); }

    ////////////////////
    reference operator[](const std::string& name) { return const_cast<reference>(*find(name)); } // o.O
    const_reference operator[](const std::string& name) const { return *find(name); }

    ////////////////////
    template<typename T>
    T to(const std::string& name, attribute::size_type n=0) const
    {
        FUNCTION_CONTEXT(ctx);
        return _M_to<T>::func(*this, name, n);
    }

    ////////////////////
    std::pair<iterator,bool> insert(const value_type& x) { return _M_attributes.insert(x); }
    std::pair<iterator,bool> insert(value_type&& x) { return _M_attributes.insert(std::move(x)); }
    void insert(std::initializer_list<value_type> x) { _M_attributes.insert(x); }

    size_type remove(const value_type& value) { return _M_attributes.erase(value); }
    size_type remove(const std::string& name) { return _M_attributes.erase(slap::attribute(name)); }

    iterator remove(const_iterator ri_0, iterator ri_1) { return _M_attributes.erase(ri_0, ri_1); }
    iterator remove(iterator ri) { return _M_attributes.erase(ri); }

    ////////////////////
    iterator begin() { return _M_attributes.begin(); }
    const_iterator begin() const { return _M_attributes.begin(); }

    iterator end() { return _M_attributes.end(); }
    const_iterator end() const { return _M_attributes.end(); }

    reverse_iterator rbegin() { return _M_attributes.rbegin(); }
    const_reverse_iterator rbegin() const { return _M_attributes.rbegin(); }

    reverse_iterator rend() { return _M_attributes.rend(); }
    const_reverse_iterator rend() const { return _M_attributes.rend(); }

    const_iterator cbegin() const { return _M_attributes.cbegin(); }
    const_iterator cend() const { return _M_attributes.cend(); }

    const_reverse_iterator crbegin() const { return _M_attributes.crbegin(); }
    const_reverse_iterator crend() const { return _M_attributes.crend(); }

    ////////////////////
    iterator find(const value_type& value) { return _M_attributes.find(value); }
    const_iterator find(const value_type& value) const { return _M_attributes.find(value); }

    iterator find(const std::string& name) { return _M_attributes.find(slap::attribute(name)); }
    const_iterator find(const std::string& name) const { return _M_attributes.find(slap::attribute(name)); }

    size_type count(const value_type& value) const { return _M_attributes.count(value); }
    size_type count(const std::string& name) const { return _M_attributes.count(slap::attribute(name)); }

private:
    std::string      _M_dn;
    slap::attributes _M_attributes;

    mutable LDAPMod** _M_mod= nullptr;

    void create_mod() const;
    void delete_mod() const;

    friend class connection;

    ////////////////////
    template<typename T>
    struct _M_to
    {
        static T func(const slap::entry& e, const std::string& name, attribute::size_type n=0)
        {
            const_iterator ri= e.find(name);
            if(ri != e.end())
                return ri->to<T>(n);
            else throw out_of_range("entry::_M_to");
        }
    };

    template<typename T>
    struct _M_to<optional<T>>
    {
        static optional<T> func(const slap::entry& e, const std::string& name, attribute::size_type n=0)
        {
            const_iterator ri= e.find(name);
            if(ri != e.end())
                return ri->to<T>(n);
            else return optional<T>();
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
        FUNCTION_CONTEXT(ctx);
        _M_uri= uri;
        _M_start_TLS= start_TLS;
        _M_open();
    }
    void open(std::string&& uri, bool start_TLS= true)
    {
        FUNCTION_CONTEXT(ctx);
        _M_uri= std::move(uri);
        _M_start_TLS= start_TLS;
        _M_open();
    }
    void close();

    void bind(const std::string& dn, const std::string& passwd= std::string())
    {
        FUNCTION_CONTEXT(ctx);
        _M_bind_dn= dn;
        _M_passwd= passwd;
        _M_bind();
    }
    void bind(std::string&& dn, const std::string& passwd= std::string())
    {
        FUNCTION_CONTEXT(ctx);
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
