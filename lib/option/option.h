///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef OPTION_H
#define OPTION_H

///////////////////////////////////////////////////////////////////////////////////////////////////
#include <vector>
#include <map>
#include <string>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace opt
{

///////////////////////////////////////////////////////////////////////////////////////////////////
enum argument
{
    no, required, optional
};

///////////////////////////////////////////////////////////////////////////////////////////////////
struct option
{
    std::string name;
    char short_name;
    argument arg;

    std::string desc;

    option(const std::string& _m_name,
        const char _m_short,
        argument _m_arg= no,
        const std::string& _m_desc= std::string()):
    name(_m_name), short_name(_m_short), arg(_m_arg), desc(_m_desc) { }

    option(const std::string& _m_name,
        argument _m_arg= no,
        const std::string& _m_desc= std::string()):
    name(_m_name), short_name(0), arg(_m_arg), desc(_m_desc) { }

    option(const char _m_short,
        argument _m_arg= no,
        const std::string& _m_desc= std::string()):
    short_name(_m_short), arg(_m_arg), desc(_m_desc) { }

    std::vector<std::string> value;
};

typedef std::vector<option> options;
typedef std::map<const std::string, const option*> option_map;

///////////////////////////////////////////////////////////////////////////////////////////////////
void parse(int argc, char* argv[], options&, int& index);

option_map map(const options&);
inline bool found(const option_map& map, const std::string& name)
{
    return map.count(name) && map.at(name)->value.size();
}

inline bool get_if_found(const option_map& map, const std::string& name, size_t index, std::string& value)
{
    option_map::const_iterator ri= map.find(name);
    if(ri!=map.end() && index < ri->second->value.size())
    {
        value= ri->second->value.at(index);
        return true;
    }
    else return false;
}

std::string usage(const options&);

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // OPTION_H
