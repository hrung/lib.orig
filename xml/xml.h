///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef XML_H
#define XML_H

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "container.h"

#include <initializer_list>
#include <vector>
#include <set>
#include <string>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace xml
{

///////////////////////////////////////////////////////////////////////////////////////////////////
void validate_name(const std::string&);
void validate_value(const std::string&);
void validate_content(const std::string&);

///////////////////////////////////////////////////////////////////////////////////////////////////
class attribute
{
public:
    attribute() = default;
    attribute(const attribute&) = default;
    attribute(attribute&&) = default;

    attribute& operator=(const attribute&) = default;
    attribute& operator=(attribute&&) = default;

    explicit attribute(const std::string& name, const std::string& value= std::string()):
        _M_name(name), _M_value(value)
    { }
    explicit attribute(std::string&& name, std::string&& value= std::string()):
        _M_name(std::move(name)), _M_value(std::move(value))
    { }

    std::string& name() noexcept { return _M_name; }
    const std::string& name() const noexcept { return _M_name; }

    std::string& value() noexcept { return _M_value; }
    const std::string& value() const noexcept { return _M_value; }

    bool empty_attribute() const noexcept { return _M_name.empty() && _M_value.empty(); }

    void validate() const
    {
        validate_name(_M_name);
        validate_value(_M_value);
    }

private:
    std::string _M_name, _M_value;

    std::string to_string() const;
    void parse(std::string& source);

    friend class tag;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
inline bool operator<(const attribute& x, const attribute& y) { return x.name() < y.name(); }

///////////////////////////////////////////////////////////////////////////////////////////////////
class tag: public container<std::set<xml::attribute>>
{
public:
    typedef container_type attributes;

public:
    tag() = default;
    tag(const tag&) = default;
    tag(tag&&) = default;

    tag& operator=(const tag&) = default;
    tag& operator=(tag&&) = default;

    tag(const std::string& name): _M_name(name)
    { }
    tag(std::string&& name):
        _M_name(std::move(name))
    { }

    tag(const std::string& name, std::initializer_list<value_type> attributes):
        _M_name(name)
    { insert(attributes); }

    tag(std::string&& name, std::initializer_list<value_type> attributes):
        _M_name(std::move(name))
    { insert(attributes); }

    ////////////////////
    const std::string& name() const noexcept { return _M_name; }
    bool empty_tag() const noexcept { return _M_name.empty() && _M_c.empty(); }

    ////////////////////
    reference attribute(const std::string& name)
    {
        iterator ri= find(name);
        if(ri == end()) throw std::out_of_range("tag::attribute()");
        return const_cast<reference>(*ri); // o.O
    }
    const_reference attribute(const std::string& name) const
    {
        const_iterator ri= find(name);
        if(ri == cend()) throw std::out_of_range("tag::attribute()");
        return *ri;
    }

    reference operator[](const std::string& name) { return attribute(name); }
    const_reference operator[](const std::string& name) const { return attribute(name); }

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

    ////////////////////
    void validate() const
    {
        validate_name(_M_name);
        for(const_reference x: _M_c) x.validate();
    }

private:
    std::string _M_name;
    enum tag_type { tag_start, tag_end, tag_empty };

    std::string to_string(tag_type type, bool nice, int ix) const;
    void parse(std::string& source, tag_type type, bool& empty);

    friend class element;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class element;
typedef std::vector<xml::element> elements;

class element
{
public:
    element() = default;
    element(const element&) = default;
    element(element&&) = default;

    element& operator=(const element&) = default;
    element& operator=(element&&) = default;

    explicit element(const std::string& name, const std::string& value= std::string()):
        _M_tag(name),
        _M_value(value)
    { }
    explicit element(std::string&& name, std::string&& value= std::string()):
        _M_tag(std::move(name)),
        _M_value(std::move(value))
    { }

    element(const std::string& name, std::initializer_list<attribute> attributes, const std::string& value= std::string()):
        _M_tag(name, attributes),
        _M_value(value)
    { }
    element(std::string&& name, std::initializer_list<attribute> attributes, std::string&& value= std::string()):
        _M_tag(std::move(name), attributes),
        _M_value(std::move(value))
    { }

    ////////////////////
    const std::string& name() const noexcept { return _M_tag.name(); }
    std::string value() const;

    xml::tag& tag() noexcept { return _M_tag; }
    const xml::tag& tag() const noexcept { return _M_tag; }

    xml::elements& children() noexcept { return _M_children; }
    const xml::elements& children() const noexcept { return _M_children; }

    bool empty_element() const noexcept { return _M_tag.empty_tag() && _M_children.empty() && _M_value.empty(); }
    bool complex() const noexcept { return !_M_children.empty(); }

    bool make_simple();

    ////////////////////
    void insert(const std::string& value)
    {
        if(!complex())
            _M_value+= value;
        else insert(element(std::string(), value));
    }
    void insert(std::string&& value)
    {
        if(!complex())
            _M_value+= value;
        else insert(element(std::string(), std::move(value)));
    }

    void insert(const element&);
    void insert(element&&);

    void validate()
    {
        _M_tag.validate();
        if(complex())
            for(element& e: _M_children) e.validate();
        else
            validate_content(_M_value);
    }

    ////////////////////
    std::string to_string(bool nice= false) const { return to_string(nice, 0); }

private:
    xml::tag _M_tag;

    std::string _M_value;
    elements _M_children;

    std::string to_string(bool nice, int ix) const;
    void parse(std::string& source);

    friend element parse(const std::string&);
    friend element parse(std::string&&);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
inline element parse(const std::string& source)
{
    element e;
    std::string x= source;

    e.parse(x);
    return e;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
inline element parse(std::string&& source)
{
    element e;
    std::string x= std::move(source);

    e.parse(x);
    return e;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // XML_H
