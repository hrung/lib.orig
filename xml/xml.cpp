///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "xml.hpp"
#include "xml_error.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace xml
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
const std::error_category& xml_category()
{
    static class xml_category instance;
    return instance;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void validate_name(const std::string& name)
{
    auto ri = name.begin();
    if( ri == name.end() || !(isalpha(*ri) || *ri == ':' || *ri == '_') ) throw xml_error(errc::syntax);

    for(++ri; ri != name.end(); ++ri)
        if( !(isalnum(*ri) || *ri == ':' || *ri == '_' || *ri == '.' || *ri == '-') )
    throw xml_error(errc::syntax);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void validate_value(const std::string& value)
{
    if(std::string::npos != value.find_first_of("'\"")) throw xml_error(errc::syntax);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void validate_content(const std::string& value)
{
    if(std::string::npos && value.find_first_of("<>")) throw xml_error(errc::syntax);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
bool parse_name(std::string& source, std::string& name)
{
    auto ri = source.begin();
    if( !(isalpha(*ri) || *ri == ':' || *ri == '_') ) return false;

    for(++ri; ri != source.end(); ++ri)
        if( !(isalnum(*ri) || *ri == ':' || *ri == '_' || *ri == '.' || *ri == '-') )
    break;

    name.assign(source.begin(), ri); source.erase(source.begin(), ri);
    return name.size();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void parse_space(std::string& source)
{
    for(auto ri = source.begin(); ri != source.end(); ++ri)
        if(!isspace(*ri))
        {
            source.erase(source.begin(), ri);
            break;
        }
}

inline void chop(std::string& source) { source.erase(0, 1); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
std::string attribute::to_string() const
{
    if(_M_name.empty())
        return std::string();
    else return " " + _M_name + "=\"" + _M_value + "\"";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void attribute::parse(std::string& source)
{
    std::string name;
    if(!parse_name(source, name)) return;
    _M_name.assign(name);

    parse_space(source);
    if(source[0] != '=') throw xml_error(errc::invalid_token);
    chop(source);

    parse_space(source);
    char quote = source[0];
    if( !(quote == '"' || quote == '\'') ) throw xml_error(errc::invalid_token);
    chop(source);

    size_t pos = source.find(quote);
    if(pos == std::string::npos) throw xml_error(errc::invalid_token);
    _M_value.assign(source, 0, pos);
    source.erase(0, ++pos);

    parse_space(source);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
inline std::string decorate(const std::string& value, bool nice, int ix)
{
    return nice ? std::string(2 * ix, ' ') + value + '\n' : value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string tag::to_string(tag_type type, bool nice, int ix) const
{
    std::string value = "<";
        if(type == tag_end) value += "/";
    value += _M_name;

    if(type == tag_start || type == tag_empty)
        for(auto ri = begin(); ri != end(); ++ri)
    value += ri->to_string();

    if(type == tag_empty) value += "/";
    value += ">";

    return decorate(value, nice, ix);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tag::parse(std::string& source, tag_type type, bool& empty)
{
    if(source[0] != '<') throw xml_error(errc::invalid_token);
    chop(source);

    if(type == tag_end)
    {
        if(source[0] != '/') throw xml_error(errc::invalid_token);
        chop(source);
    }

    std::string name;
    if(!parse_name(source, name)) throw xml_error(errc::syntax);

    if(type == tag_end)
    {
        if(name != _M_name) throw xml_error(errc::tag_mismatch);
    }
    else _M_name.assign(name);

    parse_space(source);

    if(type != tag_end) while(true)
    {
        xml::attribute x;
        x.parse(source);

        if(x.empty_attribute()) break;

        if(count(x.name())) throw xml_error(errc::duplicate_attribute);
        insert(std::move(x));
    }

    if(source[0] == '/')
    {
        if(type == tag_end) throw xml_error(errc::invalid_token);

        chop(source);
        empty = true;
    }
    if(source[0] != '>') throw xml_error(errc::unclosed_token);
    chop(source);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
std::string element::value() const
{
    if(complex())
    {
        std::string x;
        for(auto& e: _M_children) x += e.to_string();
        return x;
    }
    else return _M_value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool element::make_simple()
{
    if(_M_tag.empty_tag() && _M_children.size() == 1)
    {
        element e = std::move(_M_children[0]);

        _M_tag      = std::move(e._M_tag);
        _M_value    = std::move(e._M_value);
        _M_children = std::move(e._M_children);

        return true;
    }
    else return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void element::insert(const element& x)
{
    if(!complex() && _M_value.size())
    {
        _M_children.push_back(element(std::string(), std::move(_M_value)));
        _M_value.clear();
    }

    _M_children.push_back(x);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void element::insert(element&& x)
{
    if(!complex() && _M_value.size())
    {
        _M_children.push_back(element(std::string(), std::move(_M_value)));
        _M_value.clear();
    }

    _M_children.push_back(std::move(x));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string element::to_string(bool nice, int ix) const
{
    if(_M_value.empty() && _M_children.empty())
        return _M_tag.empty_tag() ?
    std::string() : _M_tag.to_string(tag::tag_empty, nice, ix++);

    std::string value;
    if(!_M_tag.empty_tag()) value += _M_tag.to_string(tag::tag_start, nice, ix++);

    if(complex())
        for(auto ri = _M_children.begin(); ri != _M_children.end(); ++ri)
            value += ri->to_string(nice, ix);
    else value += decorate(_M_value, nice, ix);

    if(!_M_tag.empty_tag()) value += _M_tag.to_string(tag::tag_end, nice, --ix);
    return value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void element::parse(std::string& source)
{
    while(source.size())
    {
        if(size_t pos = source.find('<'))
        {
            std::string value(source, 0, pos);
            source.erase(0, pos);
            insert(std::move(value));
        }
        else if(source[1] == '/') // end tag
        {
            bool empty;
            _M_tag.parse(source, tag::tag_end, empty);

            break;
        }
        else // start tag
        {
            element e;

            bool empty = false;
            e.tag().parse(source, tag::tag_start, empty);

            if(!empty) e.parse(source);

            insert(std::move(e));
        }
    }
    make_simple();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}
