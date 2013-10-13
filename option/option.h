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

typedef std::vector<std::string> values;

///////////////////////////////////////////////////////////////////////////////////////////////////
class option
{
public:
    option(const std::string& name, const char abbr, opt::argument arg= no, const std::string& desc= std::string()):
        _M_name(name), _M_abbr(abbr), _M_arg(arg), _M_desc(desc)
    { }
    option(std::string&& name, const char abbr, opt::argument arg= no, std::string&& desc= std::string()):
        _M_name(std::move(name)), _M_abbr(abbr), _M_arg(arg), _M_desc(std::move(desc))
    { }

    option(const std::string& name, opt::argument arg= no, const std::string& desc= std::string()):
        _M_name(name), _M_abbr(0), _M_arg(arg), _M_desc(desc)
    { }
    option(std::string&& name, opt::argument arg= no, std::string&& desc= std::string()):
        _M_name(std::move(name)), _M_abbr(0), _M_arg(arg), _M_desc(std::move(desc))
    { }

    option(const char abbr, opt::argument arg= no, const std::string& desc= std::string()):
        _M_abbr(abbr), _M_arg(arg), _M_desc(desc)
    { }
    option(const char abbr, opt::argument arg= no, std::string&& desc= std::string()):
        _M_abbr(abbr), _M_arg(arg), _M_desc(std::move(desc))
    { }

    ////////////////////
    std::string name() const { return _M_name; }
    char abbr() const { return _M_abbr; }
    opt::argument argument() const { return _M_arg; }
    std::string desc() const { return _M_desc; }

    opt::values& values() { return _M_values; }
    const opt::values& values() const { return _M_values; }

    std::string value(size_t index=0) const { return _M_values.at(index); }

    ////////////////////
    template<typename T>
    T to(size_t index=0) const { return convert::to<T>(_M_values.at(index)); }

private:
    std::string _M_name;
    char _M_abbr;
    opt::argument _M_arg;

    std::string _M_desc;

    opt::values _M_values;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class options
{
public:
    options() { }
    options(std::initializer_list<option> opts) { for(auto&& opt: opts) insert(opt); }

    void parse(int argc, char* argv[], int& index);

    ////////////////////
    bool empty() const { return _M_map.empty(); }
    int   size() const { return _M_map.size(); }
    void clear() { _M_map.clear(); }

    const option* find(const std::string& name) const
    {
        auto ri= _M_map.find(name);
        return ( ri!=_M_map.end() && ri->second.values().size() )? &ri->second: nullptr;
    }
    int count(const std::string& name) const { return _M_map.count(name); }

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

    ////////////////////
    bool insert(const option& opt) { return _M_map.insert(std::make_pair(name(opt), opt)).second; }
    bool insert(option&& opt)      { return _M_map.insert(std::make_pair(name(opt), std::move(opt))).second; }
    bool erase(const std::string& name) { return _M_map.erase(name); }

    std::string usage();

private:
    std::map<std::string, option> _M_map;

    static std::string name(const option& opt)
    {
        return opt.name().empty()?
            std::string()+ opt.abbr():
        opt.name();
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // OPTION_H
