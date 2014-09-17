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
#include "xml_error.h"

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

    std::string& name() { return _M_name; }
    const std::string& name() const { return _M_name; }

    std::string& value() { return _M_value; }
    const std::string& value() const { return _M_value; }

    bool empty_attribute() const { return _M_name.empty() && _M_value.empty(); }

    void validate() const
    {
        validate_name(_M_name);
        validate_value(_M_value);
    }

private:
    std::string _M_name, _M_value;

    std::string _M_write() const;
    void _M_parse(std::string& source);

    friend class tag;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
inline bool operator<(const attribute& x, const attribute& y) { return x.name() < y.name(); }

///////////////////////////////////////////////////////////////////////////////////////////////////
class tag: public container< std::set<xml::attribute> >
{
public:
    typedef container_type attributes;

public:
    tag() = default;
    tag(const tag&) = default;
    tag(tag&&) = default;

    tag& operator=(const tag&) = default;
    tag& operator=(tag&&) = default;

    tag(const std::string& name):
        _M_name(name)
    { }
    tag(std::string&& name):
        _M_name(std::move(name))
    { }

    tag(const std::string& name, std::initializer_list<attribute> x):
        _M_name(name), _M_c(x)
    { }
    tag(std::string&& name, std::initializer_list<attribute> x):
        _M_name(std::move(name)), _M_c(x)
    { }

    ////////////////////
    const std::string name() const { return _M_name; }
    bool empty_tag() const { return _M_name.empty() && _M_c.empty(); }

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
    std::pair<iterator,bool> insert(const value_type& x) { return _M_c.insert(x); }
    std::pair<iterator,bool> insert(value_type&& x) { return _M_c.insert(std::move(x)); }
    void insert(std::initializer_list<value_type> x) { _M_c.insert(x); }

    size_type remove(const value_type& value) { return _M_c.erase(value); }
    size_type remove(const std::string& name) { return _M_c.erase(xml::attribute(name)); }

    iterator remove(const_iterator ri_0, iterator ri_1) { return _M_c.erase(ri_0, ri_1); }
    iterator remove(iterator ri) { return _M_c.erase(ri); }

    ////////////////////
    iterator find(const value_type& value) { return _M_c.find(value); }
    const_iterator find(const value_type& value) const { return _M_c.find(value); }

    iterator find(const std::string& name) { return _M_c.find(xml::attribute(name)); }
    const_iterator find(const std::string& name) const { return _M_c.find(xml::attribute(name)); }

    size_type count(const value_type& value) const { return _M_c.count(value); }
    size_type count(const std::string& name) const { return _M_c.count(xml::attribute(name)); }

    ////////////////////
    void validate() const
    {
        validate_name(_M_name);
        for(const_reference x: _M_c) x.validate();
    }

private:
    std::string _M_name;
    enum tag_type { tag_start, tag_end, tag_empty };

    std::string _M_write(tag_type type, bool nice= false, int ix=0) const;
    void _M_parse(std::string& source, tag_type type, bool& empty);

    friend class element;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class element;
typedef std::vector<element> elements;

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

    element(const std::string& name, std::initializer_list<attribute> x, const std::string& value= std::string()):
        _M_tag(name, x),
        _M_value(value)
    { }
    element(std::string&& name, std::initializer_list<attribute> x, std::string&& value= std::string()):
        _M_tag(std::move(name), x),
        _M_value(std::move(value))
    { }

    ////////////////////
    std::string name() const { return _M_tag.name(); }
    std::string value() const;

    xml::tag& tag() { return _M_tag; }
    const xml::tag& tag() const { return _M_tag; }

    xml::elements& children() { return _M_children; }
    const xml::elements& children() const { return _M_children; }

    bool empty_element() const { return _M_tag.empty_tag() && _M_children.empty() && _M_value.empty(); }
    bool complex() const { return !_M_children.empty(); }

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
    std::string write(bool nice= false) const { return _M_write(nice, 0); }

private:
    xml::tag _M_tag;

    std::string _M_value;
    elements _M_children;

    std::string _M_write(bool nice= false, int ix=0) const;
    void _M_parse(std::string& source);

    friend element parse(const std::string& source);
    friend element parse(std::string&& source);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
inline element parse(const std::string& source)
{
    element e;
    std::string copy= source;

    e._M_parse(copy);
    return e;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
inline element parse(std::string&& source)
{
    element e;
    std::string move= std::move(source);

    e._M_parse(move);
    return e;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // XML_H
