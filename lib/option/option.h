///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef OPTION_H
#define OPTION_H

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "convert.h"

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

    ////////////////////
    std::vector<std::string> value;

    template<typename T>
    T to(size_t index=0) const { return convert::to<T>(value.at(index)); }

    template<typename T>
    bool get(T& value, size_t index=0) const
    {
        try
        {
            value= convert::to<T>(value.at(index));
            return true;
        }
        catch(...) { return false; }
    }
};

typedef std::vector<option> options;
typedef std::map<const std::string, const option*> option_map;

///////////////////////////////////////////////////////////////////////////////////////////////////
void parse(int argc, char* argv[], options&, int& index);

option_map map(const opt::options& options);

inline const option* find(const option_map& map, const std::string& name)
{
    auto ri= map.find(name);
    return (ri!=map.end() && ri->second->value.size())? ri->second: nullptr;
}

template<typename T>
T to(const option_map& map, const std::string& name, size_t index=0)
{
    const opt::option* option= find(map, name);
    if(option)
        return option->to<T>(index);
    else throw std::out_of_range("opt::to");
}

template<typename T>
bool get(const option_map& map, const std::string& name, T& value, size_t index=0)
{
    const opt::option* option= find(map, name);
    return option? option->get(value, index): false;
}

std::string usage(const options&);

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // OPTION_H
