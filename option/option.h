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

#include <initializer_list>
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
class option
{
public:
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
    std::string name;
    char short_name;
    argument arg;

    std::string desc;

    std::vector<std::string> values;

    template<typename T>
    T to(size_t index=0) const { return convert::to<T>(values.at(index)); }

    std::string name_or_short() const { return name.empty()? (std::string()+ short_name): name; }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class options
{
public:
    options() { }
    options(std::initializer_list<option> opts) { for(auto&& opt: opts) insert(opt); }

    bool insert(const option& opt) { return _M_map.insert(std::make_pair(opt.name_or_short(), opt)).second; }
    bool insert(option&& opt)      { return _M_map.insert(std::make_pair(opt.name_or_short(), opt)).second; }
    bool erase(const std::string& name) { return _M_map.erase(name); }

    void parse(int argc, char* argv[], int& index);

    ////////////////////
    bool empty() const { return _M_map.empty(); }
    int   size() const { return _M_map.size(); }
    void clear() { _M_map.clear(); }

    int count(const std::string& name) const { return _M_map.count(name); }
    const option* find(const std::string& name) const;

    ////////////////////
    template<typename T>
    bool get(T& var, const std::string& name, size_t index=0)
    {
        const option* popt= find(name);
        if(popt)
        {
            var= popt->to<T>(index);
            return true;
        }
        else return false;
    }

    std::string usage();

private:
    std::map<std::string, option> _M_map;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // OPTION_H
