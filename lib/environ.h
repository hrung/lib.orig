///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ENVIRON_H
#define ENVIRON_H

///////////////////////////////////////////////////////////////////////////////////////////////////
#include <string>
#include <utility>
#include <map>
#include <cstdlib>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace app
{

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

private:
    container_type _M_c;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // ENVIRON_H
