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
    attribute() { }

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

    bool empty() const { return _M_name.empty() && _M_value.empty(); }

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

typedef std::set<attribute> attributes;

///////////////////////////////////////////////////////////////////////////////////////////////////
class tag
{
public:
    tag() { }

    tag(const std::string& name): _M_name(name) { }
    tag(std::string&& name): _M_name(std::move(name)) { }

    tag(const std::string& name, std::initializer_list<attribute> attributes):
        _M_name(name),
        _M_attributes(attributes)
    { }
    tag(std::string&& name, std::initializer_list<attribute> attributes):
        _M_name(std::move(name)),
        _M_attributes(attributes)
    { }

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
    const std::string name() const { return _M_name; }

    bool empty() const { return _M_attributes.empty(); }
    size_type size() const { return _M_attributes.size(); }
    void clear() { _M_attributes.clear(); }

    ////////////////////
    reference attribute(const std::string& name)
    {
        iterator ri= find(name);
        if(ri == end()) throw out_of_range("tag::attribute()");
        return const_cast<reference>(*ri); // o.O
    }
    const_reference attribute(const std::string& name) const
    {
        const_iterator ri= find(name);
        if(ri == cend()) throw out_of_range("tag::attribute()");
        return *ri;
    }

    reference operator[](const std::string& name) { return attribute(name); }
    const_reference operator[](const std::string& name) const { return attribute(name); }

    ////////////////////
    std::pair<iterator,bool> insert(const value_type& x) { return _M_attributes.insert(x); }
    std::pair<iterator,bool> insert(value_type&& x) { return _M_attributes.insert(std::move(x)); }
    void insert(std::initializer_list<value_type> x) { _M_attributes.insert(x); }

    size_type remove(const value_type& value) { return _M_attributes.erase(value); }
    size_type remove(const std::string& name) { return _M_attributes.erase(xml::attribute(name)); }

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

    iterator find(const std::string& name) { return _M_attributes.find(xml::attribute(name)); }
    const_iterator find(const std::string& name) const { return _M_attributes.find(xml::attribute(name)); }

    size_type count(const value_type& value) const { return _M_attributes.count(value); }
    size_type count(const std::string& name) const { return _M_attributes.count(xml::attribute(name)); }

    ////////////////////
    void validate() const
    {
        validate_name(_M_name);
        for(const_reference x: _M_attributes) x.validate();
    }

private:
    std::string _M_name;
    xml::attributes _M_attributes;

    enum tag_type { start_tag, end_tag, empty_tag };

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
    element() { }

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
    std::string name() const { return _M_tag.name(); }
    std::string value() const;

    xml::tag& tag() { return _M_tag; }
    const xml::tag& tag() const { return _M_tag; }

    xml::elements& children() { return _M_children; }
    const xml::elements& children() const { return _M_children; }

    bool empty() const { return _M_tag.empty() && _M_children.empty() && _M_value.empty(); }
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
