///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ENVIRON_H
#define ENVIRON_H

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "errno_error.h"

#include <string>
#include <map>
#include <memory>
#include <iterator>

#include <unistd.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace app
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
class environ
{
public:
    typedef std::string name_type;
    typedef std::string value_type;

    typedef std::map<name_type, value_type> container_type;
    typedef container_type::size_type size_type;

    typedef container_type::iterator iterator;
    typedef container_type::const_iterator const_iterator;
    typedef container_type::reverse_iterator reverse_iterator;
    typedef container_type::const_reverse_iterator const_reverse_iterator;

public:
    environ() = default;

    environ(const environ&) = default;
    environ(environ&&) = default;

    environ& operator=(const environ&) = default;
    environ& operator=(environ&&) = default;

    ////////////////////
    value_type& get(const name_type& name) { return _M_c.at(name); }
    const value_type& get(const name_type& name) const { return _M_c.at(name); }

    void set(const name_type& name, const value_type& value) { _M_c[name]= value; }
    void set(name_type&& name, value_type&& value) { _M_c[std::move(name)]= std::move(value); }
    void set(const name_type& name, value_type&& value) { _M_c[name]= std::move(value); }
    void set(name_type&& name, const value_type& value) { _M_c[std::move(name)]= value; }

    void reset(const name_type& name) { _M_c.erase(name); }

    ////////////////////
    void clear() noexcept { _M_c.clear(); }
    bool empty() const noexcept { return _M_c.empty(); }

    size_type size() const noexcept { return _M_c.size(); }
    size_type count(const name_type& name) const { return _M_c.count(name); }

    iterator find(const name_type& name) { return _M_c.find(name); }
    const_iterator find(const name_type& name) const { return _M_c.find(name); }

    ////////////////////
    iterator begin() noexcept { return _M_c.begin(); }
    const_iterator begin() const noexcept { return _M_c.begin(); }

    iterator end() noexcept { return _M_c.end(); }
    const_iterator end() const noexcept { return _M_c.end(); }

    reverse_iterator rbegin() noexcept { return _M_c.rbegin(); }
    const_reverse_iterator rbegin() const noexcept { return _M_c.rbegin(); }

    reverse_iterator rend() noexcept { return _M_c.rend(); }
    const_reverse_iterator rend() const noexcept { return _M_c.rend(); }

    const_iterator cbegin() const noexcept { return _M_c.begin(); }
    const_iterator cend() const noexcept { return _M_c.end(); }

    const_reverse_iterator crbegin() const noexcept { return _M_c.rbegin(); }
    const_reverse_iterator crend() const noexcept { return _M_c.rend(); }

    ////////////////////
    static environ from_charpp(char*[]);

private:
    container_type _M_c;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace this_environ
{

///////////////////////////////////////////////////////////////////////////////////////////////////
typedef std::string name_type;
typedef std::string value_type;
typedef size_t size_type;

class iterator;
typedef iterator const_iterator;
typedef std::reverse_iterator<iterator> reverse_iterator;
typedef std::reverse_iterator<iterator> const_reverse_iterator;

///////////////////////////////////////////////////////////////////////////////////////////////////
name_type name(char*, bool* found);
value_type value(char*, bool* found);

template<typename Iterator>
name_type name(Iterator ri, bool* found= nullptr) { return name(*ri, found); }

template<typename Iterator>
value_type value(Iterator ri, bool* found= nullptr) { return value(*ri, found); }

///////////////////////////////////////////////////////////////////////////////////////////////////
class iterator: public std::iterator<std::bidirectional_iterator_tag, char*, ptrdiff_t, char**, char*>
{
public:
    iterator() noexcept: _M_p(nullptr) { }
    iterator(pointer x) noexcept: _M_p(x) { }

    iterator(const iterator& x) noexcept { _M_p= x._M_p; }
    iterator& operator=(const iterator& x) noexcept { _M_p= x._M_p; return (*this); }

    reference operator*() const noexcept { return _M_p? *_M_p: nullptr; }

    this_environ::name_type name(bool* found= nullptr) const { return this_environ::name(*this, found); }
    this_environ::value_type value(bool* found= nullptr) const { return this_environ::value(*this, found); }

    iterator& operator++() noexcept { ++_M_p; return (*this); }
    iterator operator++(int) noexcept
    {
        iterator x(*this);
        operator++();
        return x;
    }

    iterator& operator--() noexcept { --_M_p; return (*this); }
    iterator operator--(int) noexcept
    {
        iterator x(*this);
        operator--();
        return x;
    }

    friend bool operator==(const iterator& x, const iterator& y) noexcept { return x._M_p == y._M_p; }
    friend bool operator!=(const iterator& x, const iterator& y) noexcept { return x._M_p != y._M_p; }
    friend size_type size() noexcept;

private:
    pointer _M_p;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
value_type get(const name_type& name, bool* found= nullptr) noexcept;
void set(const name_type& name, const value_type& value, bool over= true);
void reset(const std::string& name);

////////////////////
inline iterator begin() noexcept { return iterator(::environ); }
inline iterator end() noexcept
{
    iterator ri= begin(); while(*ri) ++ri;
    return ri;
}

inline reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
inline reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

inline const_iterator cbegin() noexcept { return begin(); }
inline const_iterator cend() noexcept { return end(); }

inline const_reverse_iterator crbegin() noexcept { return rbegin(); }
inline const_reverse_iterator crend() noexcept { return rend(); }

////////////////////
inline bool empty() noexcept { return *begin() == nullptr; }
inline size_type size() noexcept { return end()._M_p - begin()._M_p; }

size_type count(const name_type& name) noexcept;

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // ENVIRON_H
