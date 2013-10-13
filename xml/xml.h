///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013 Dimitry Ishenko
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

    std::string  name() const { return _M_name; }
    std::string value() const { return _M_value; }

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

    ////////////////////
    const std::string name() const { return _M_name; }

    bool empty() const { return _M_name.empty() && _M_attributes.empty(); }

    xml::attributes& attributes() { return _M_attributes; }
    const xml::attributes& attributes() const { return _M_attributes; }

    const attribute* find(const std::string& name) const
    {
        auto ri= _M_attributes.find(attribute(name));
        return ri!=_M_attributes.end()? &*ri: nullptr;
    }
    int count(const std::string& name) const { return _M_attributes.count(attribute(name)); }

    std::string value(const std::string& name) const
    {
        if(auto ri= find(name))
            return ri->value();
        else throw std::out_of_range("tag::value");
    }

    ////////////////////
    bool get(std::string& var, const std::string& name)
    {
        if(auto value= find(name))
        {
            var= value->value();
            return true;
        }
        else return false;
    }

    ////////////////////
    bool insert(const attribute& x) { return _M_attributes.insert(x).second; }
    bool insert(attribute&& x) { return _M_attributes.insert(std::move(x)).second; }
    void insert(std::initializer_list<attribute> x) { return _M_attributes.insert(x); }

    void validate() const
    {
        validate_name(_M_name);
        for(auto& ri: _M_attributes) ri.validate();
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

    bool promote();

    ////////////////////
    bool insert(const attribute& x) { return _M_tag.insert(x); }
    bool insert(attribute&& x) { return _M_tag.insert(x); }
    void insert(std::initializer_list<attribute> x) { _M_tag.insert(x); }

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
        else validate_content(_M_value);
    }

    ////////////////////
    std::string write(bool nice= false) const { return _M_write(nice, 0); }
    void parse(const std::string& source)
    {
        std::string copy= source;
        _M_parse(copy);
    }
    void parse(std::string&& source)
    {
        std::string copy= std::move(source);
        _M_parse(copy);
    }

private:
    xml::tag _M_tag;

    std::string _M_value;
    elements _M_children;

    std::string _M_write(bool nice= false, int ix=0) const;
    void _M_parse(std::string& source);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
inline element parse(const std::string& source)
{
    element e;
    e.parse(source);
    return e;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
inline element parse(std::string&& source)
{
    element e;
    e.parse(std::move(source));
    return e;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // XML_H
