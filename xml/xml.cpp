///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "xml.h"

#include "stream.h"
#include "except.h"

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
    auto ri= name.begin();
    if( ri==name.end() || !(isalpha(*ri) || *ri==':' || *ri=='_') ) throw xml_except(errc::syntax);

    for(++ri; ri!=name.end(); ++ri)
        if( !(isalnum(*ri) || *ri==':' || *ri=='_' || *ri=='.' || *ri=='-') )
    throw xml_except(errc::syntax);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void validate_value(const std::string& value)
{
    if(std::string::npos != value.find_first_of(R"('")")) throw xml_except(errc::syntax);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void validate_content(const std::string& value)
{
    if(std::string::npos && value.find_first_of("<>")) throw xml_except(errc::syntax);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
bool parse_name(std::string& source, std::string& name)
{
    auto ri= source.begin();
    if( !(isalpha(*ri) || *ri==':' || *ri=='_') ) return false;

    for(++ri; ri!=source.end(); ++ri)
        if( !(isalnum(*ri) || *ri==':' || *ri=='_' || *ri=='.' || *ri=='-') )
    break;

    name.assign(source.begin(), ri); source.erase(source.begin(), ri);
    return name.size();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void parse_space(std::string& source)
{
    for(auto ri= source.begin(); ri!=source.end(); ++ri)
        if(!isspace(*ri))
        {
            source.erase(source.begin(), ri);
            break;
        }
}

inline void chop(std::string& source) { source.erase(0, 1); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
std::string attribute::_M_write() const
{
    if(_M_name.empty())
        return std::string();
    else return " "+ _M_name+ "=\""+ _M_value+ "\"";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void attribute::_M_parse(std::string& source)
{
    std::string name;
    if(!parse_name(source, name)) return;
    _M_name.assign(name);

    parse_space(source);
    if(source[0] != '=') throw xml_except(errc::invalid_token);
    chop(source);

    parse_space(source);
    char quote= source[0];
    if( !(quote=='"' || quote=='\'') ) throw xml_except(errc::invalid_token);
    chop(source);

    size_t pos= source.find(quote);
    if(pos==std::string::npos) throw xml_except(errc::invalid_token);
    _M_value.assign(source, 0, pos);
    source.erase(0, ++pos);

    parse_space(source);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
inline std::string decorate(const std::string& value, bool nice, int ix)
{
    return nice? std::string(2*ix, ' ')+ value+ '\n': value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string tag::_M_write(tag_type type, bool nice, int ix) const
{
    std::string value("<");
        if(type==end_tag) value+= "/";
    value+= _M_name;

    if(type==start_tag || type==empty_tag)
        for(auto ri= _M_attributes.begin(); ri != _M_attributes.end(); ++ri)
    value+= ri->_M_write();

    if(type==empty_tag) value+= "/";
    value+= ">";

    return decorate(value, nice, ix);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tag::_M_parse(std::string& source, tag_type type, bool& empty)
{
    if(source[0] != '<') throw xml_except(errc::invalid_token);
    chop(source);

    if(type==end_tag)
    {
        if(source[0] != '/') throw xml_except(errc::invalid_token);
        chop(source);
    }

    std::string name;
    if(!parse_name(source, name)) throw xml_except(errc::syntax);

    if(type==end_tag)
    {
        if(name!=_M_name) throw xml_except(errc::tag_mismatch);
    }
    else _M_name.assign(name);

    parse_space(source);

    if(type!=end_tag) while(true)
    {
        attribute x;
        x._M_parse(source);

        if(x.empty()) break;

        if(count(x.name())) throw xml_except(errc::duplicate_attribute);
        insert(std::move(x));
    }

    if(source[0] == '/')
    {
        if(type==end_tag) throw xml_except(errc::invalid_token);

        chop(source);
        empty= true;
    }
    if(source[0] != '>') throw xml_except(errc::unclosed_token);
    chop(source);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
std::string element::value() const
{
    if(complex())
    {
        std::string x;
        for(auto& e: _M_children) x+= e._M_write();
        return x;
    }
    else return _M_value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool element::promote()
{
    if(_M_tag.empty() && _M_children.size()==1)
    {
        element e= std::move(_M_children[0]);

        _M_tag=      std::move(e._M_tag);
        _M_value=    std::move(e._M_value);
        _M_children= std::move(e._M_children);

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
std::string element::_M_write(bool nice, int ix) const
{
    if(_M_value.empty() && _M_children.empty())
        return _M_tag.empty()?
    std::string(): _M_tag._M_write(tag::empty_tag, nice, ix++);

    std::string value;
    if(!_M_tag.empty()) value+= _M_tag._M_write(tag::start_tag, nice, ix++);

    if(complex())
        for(auto ri= _M_children.begin(); ri != _M_children.end(); ++ri)
            value+= ri->_M_write(nice, ix);
    else value+= decorate(_M_value, nice, ix);

    if(!_M_tag.empty()) value+= _M_tag._M_write(tag::end_tag, nice, --ix);
    return value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void element::_M_parse(std::string& source)
{
    while(source.size())
    {
        if(size_t pos= source.find('<'))
        {
            std::string value(source, 0, pos);
            source.erase(0, pos);
            insert(std::move(value));
        }
        else if(source[1]=='/') // end tag
        {
            bool empty;
            _M_tag._M_parse(source, tag::end_tag, empty);

            break;
        }
        else // start tag
        {
            element e;

            bool empty= false;
            e.tag()._M_parse(source, tag::start_tag, empty);

            if(!empty) e._M_parse(source);

            insert(std::move(e));
        }
    }
    promote();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}
