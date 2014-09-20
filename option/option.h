///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef OPTION_H
#define OPTION_H

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "convert.h"
#include "tern.h"
#include "container.h"

#include <stdexcept>
#include <functional>
#include <initializer_list>
#include <vector>
#include <string>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace app
{

///////////////////////////////////////////////////////////////////////////////////////////////////
constexpr uncertain_t optional= uncertain;

///////////////////////////////////////////////////////////////////////////////////////////////////
class option
{
public:
    ////////////////////
    ///
    /// \param longname long name (eg, --foo)
    /// \param name     short name (eg, -f)
    /// \param desc     description
    /// \param once     this option can only be specified once
    ///
    /// option constuctor (for options without argument)
    ///
    /// \example option("foo", 'f', "Option foo");
    /// \example option("bar", "Option bar");
    /// \example option('b', "Option baz", true);
    ///
    option(const std::string& longname, const char name, const std::string& desc, const bool once= false):
        _M_longname(longname), _M_name(name), _M_once(once), _M_desc(desc)
    { }
    option(std::string&& longname, const char name, std::string&& desc, const bool once= false):
        _M_longname(std::move(longname)), _M_name(name), _M_once(once), _M_desc(std::move(desc))
    { }
    option(const std::string& longname, const std::string& desc, const bool once= false):
        _M_longname(longname), _M_once(once), _M_desc(desc)
    { }
    option(std::string&& longname, std::string&& desc, const bool once= false):
        _M_longname(std::move(longname)), _M_once(once), _M_desc(std::move(desc))
    { }
    option(const char name, const std::string& desc, const bool once= false):
        _M_name(name), _M_once(once), _M_desc(desc)
    { }
    option(const char name, std::string&& desc, const bool once= false):
        _M_name(name), _M_once(once), _M_desc(std::move(desc))
    { }

    ////////////////////
    ///
    /// \param longname long name (eg, --foo)
    /// \param name     short name (eg, -f)
    /// \param desc     description
    /// \param value    variable to store the value of the argument
    ///                 (if the variable is of type vector<T>, the option can be specified more than once)
    /// \param arg      whether option has an argument (must be one of false, true or optional)
    ///
    /// option constuctor
    ///
    /// \example vector<int> arg1; option("foo", 'f', "Option foo", arg1);
    /// \example bool arg2;        option("bar", "Option bar", arg2);
    /// \example string arg3;      option('b', "Option baz", arg3, optional);
    ///
    template<typename T>
    option(const std::string& longname, const char name, const std::string& desc, T& value, const tern arg= true):
        _M_longname(longname), _M_name(name), _M_arg(arg? true: uncertain), _M_once(_M_Arg<T>::once), _M_desc(desc),
        _M_assign(std::bind(_M_Arg<T>::assign, std::ref(value), std::placeholders::_1))
    { }
    template<typename T>
    option(std::string&& longname, const char name, std::string&& desc, T& value, const tern arg= true):
        _M_longname(std::move(longname)), _M_name(name), _M_arg(arg? true: uncertain), _M_once(_M_Arg<T>::once), _M_desc(std::move(desc)),
        _M_assign(std::bind(_M_Arg<T>::assign, std::ref(value), std::placeholders::_1))
    { }
    template<typename T>
    option(const std::string& longname, const std::string& desc, T& value, const tern arg= true):
        _M_longname(longname), _M_arg(arg? true: uncertain), _M_once(_M_Arg<T>::once), _M_desc(desc),
        _M_assign(std::bind(_M_Arg<T>::assign, std::ref(value), std::placeholders::_1))
    { }
    template<typename T>
    option(std::string&& longname, std::string&& desc, T& value, const tern arg= true):
        _M_longname(std::move(longname)), _M_arg(arg? true: uncertain), _M_once(_M_Arg<T>::once), _M_desc(std::move(desc)),
        _M_assign(std::bind(_M_Arg<T>::assign, std::ref(value), std::placeholders::_1))
    { }
    template<typename T>
    option(const char name, const std::string& desc, T& value, const tern arg= true):
        _M_name(name), _M_arg(arg? true: uncertain), _M_once(_M_Arg<T>::once), _M_desc(desc),
        _M_assign(std::bind(_M_Arg<T>::assign, std::ref(value), std::placeholders::_1))
    { }
    template<typename T>
    option(const char name, std::string&& desc, T& value, const tern arg= true):
        _M_name(name), _M_arg(arg? true: uncertain), _M_once(_M_Arg<T>::once), _M_desc(std::move(desc)),
        _M_assign(std::bind(_M_Arg<T>::assign, std::ref(value), std::placeholders::_1))
    { }

    ///////////////////
    option() = default;
    option(const option&) = default;
    option(option&&) = default;

    option& operator=(const option&) = default;
    option& operator=(option&&) = default;

    ///////////////////
    std::string longname() const { return _M_longname; }
    char name()    const { return _M_name; }

    tern arg()     const { return _M_arg; }
    bool has_arg() const { return _M_has_arg; }

    bool once()    const { return _M_once; }
    int count()    const { return _M_count; }

    std::string desc() const { return _M_desc; }

    operator bool() { return count(); }

protected:
    std::string _M_longname;
    char _M_name=0;

    tern _M_arg= false;
    bool _M_has_arg= false;

    bool _M_once= true;
    int _M_count=0;

    std::string _M_desc;
    std::function<void(const std::string&)> _M_assign= nullptr;

    friend class options;

    ///////////////////
    template<typename T>
    struct _M_Arg
    {
        static void assign(T& value, const std::string& source) { value= convert::to<T>(source); }
        static constexpr bool once= true;
    };
    template<typename T>
    struct _M_Arg<std::vector<T>>
    {
        static void assign(std::vector<T>& value, const std::string& source) { value.push_back(convert::to<T>(source)); }
        static constexpr bool once= false;
    };
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class options: public container<std::vector<option>>
{
public:
    options() = default;
    options(const options&) = default;
    options(options&&) = default;

    options(std::initializer_list<value_type> x) { insert(x); }

    options& operator=(const options&) = default;
    options& operator=(options&&) = default;

    ////////////////////
    reference operator[](size_type n) { return _M_c.at(n); }
    const_reference operator[](size_type n) const { return _M_c.at(n); }

    reference operator[](const std::string& longname)
    {
        iterator ri= find(longname);
        if(ri == end()) throw std::out_of_range("option::operator[]");
        return *ri;
    }
    const_reference operator[](const std::string& longname) const
    {
        const_iterator ri= find(longname);
        if(ri == end()) throw std::out_of_range("option::operator[]");
        return *ri;
    }

    reference operator[](const char name)
    {
        iterator ri= find(name);
        if(ri == end()) throw std::out_of_range("option::operator[]");
        return *ri;
    }
    const_reference operator[](const char name) const
    {
        const_iterator ri= find(name);
        if(ri == end()) throw std::out_of_range("option::operator[]");
        return *ri;
    }

    ////////////////////
    using container::insert;
    using container::erase;

    iterator erase(const std::string& longname) { return _M_c.erase(find(longname)); }
    iterator erase(const char name) { return _M_c.erase(find(name)); }

    ////////////////////
    iterator find(const std::string& longname)
    {
        for(auto ri= begin(); ri!=end(); ++ri) if(ri->longname()==longname) return ri;
        return end();
    }
    const_iterator find(const std::string& longname) const
    {
        for(auto ri= begin(); ri!=end(); ++ri) if(ri->longname()==longname) return ri;
        return end();
    }
    iterator find(const char name)
    {
        for(auto ri= begin(); ri!=end(); ++ri) if(ri->name()==name) return ri;
        return end();
    }
    const_iterator find(const char name) const
    {
        for(auto ri= begin(); ri!=end(); ++ri) if(ri->name()==name) return ri;
        return end();
    }

    ////////////////////
    void parse(int argc, char* argv[], int& index);
    std::string usage();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // OPTION_H
