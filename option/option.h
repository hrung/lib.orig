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
    /// \param long_name    long name (eg, --foo)
    /// \param name         short name (eg, -f)
    /// \param description  description
    /// \param once         this option can only be specified once
    ///
    /// option constuctor (for options without argument)
    ///
    /// \example option("foo", 'f', "Option foo");
    /// \example option("bar", "Option bar");
    /// \example option('b', "Option baz", true);
    ///
    template<typename LongNameType, typename DescriptionType>
    option(LongNameType&& long_name, const char name, DescriptionType&& description, const bool once= false):
        _M_long(std::forward<LongNameType>(long_name)),
        _M_name(name),
        _M_once(once),
        _M_desc(std::forward<DescriptionType>(description))
    { }

    template<typename LongNameType, typename DescriptionType>
    option(LongNameType&& long_name, DescriptionType&& description, const bool once= false):
        _M_long(std::forward<LongNameType>(long_name)),
        _M_once(once),
        _M_desc(std::forward<DescriptionType>(description))
    { }

    template<typename DescriptionType>
    option(const char name, DescriptionType&& description, const bool once= false):
        _M_name(name),
        _M_once(once),
        _M_desc(std::forward<DescriptionType>(description))
    { }

    ////////////////////
    ///
    /// \param long_name    long name (eg, --foo)
    /// \param name         short name (eg, -f)
    /// \param description  description
    /// \param value        variable to store the value of the argument
    ///                     (if the variable is of type vector<T>, the option can be specified more than once)
    /// \param arg          whether option has an argument (must be one of false, true or optional)
    ///
    /// option constuctor
    ///
    /// \example vector<int> arg1; option("foo", 'f', "Option foo", arg1);
    /// \example bool arg2;        option("bar", "Option bar", arg2);
    /// \example string arg3;      option('b', "Option baz", arg3, optional);
    ///
    template<typename LongNameType, typename DescriptionType, typename ValueType>
    option(LongNameType&& long_name, const char name, DescriptionType&& description, ValueType& value, const tern arg= true):
        _M_long(std::forward<LongNameType>(long_name)),
        _M_name(name),
        _M_arg(arg? true: uncertain),
        _M_once(_M_Arg<ValueType>::once),
        _M_desc(std::forward<DescriptionType>(description)),
        _M_assign(std::bind(_M_Arg<ValueType>::assign, std::ref(value), std::placeholders::_1))
    { }

    template<typename LongNameType, typename DescriptionType, typename ValueType>
    option(LongNameType&& long_name, DescriptionType&& description, ValueType& value, const tern arg= true):
        _M_long(std::forward<LongNameType>(long_name)),
        _M_arg(arg? true: uncertain),
        _M_once(_M_Arg<ValueType>::once),
        _M_desc(std::forward<DescriptionType>(description)),
        _M_assign(std::bind(_M_Arg<ValueType>::assign, std::ref(value), std::placeholders::_1))
    { }

    template<typename DescriptionType, typename ValueType>
    option(const char name, DescriptionType&& description, ValueType& value, const tern arg= true):
        _M_name(name),
        _M_arg(arg? true: uncertain),
        _M_once(_M_Arg<ValueType>::once),
        _M_desc(std::forward<DescriptionType>(description)),
        _M_assign(std::bind(_M_Arg<ValueType>::assign, std::ref(value), std::placeholders::_1))
    { }

    ///////////////////
    option() = default;
    option(const option&) = default;
    option(option&&) = default;

    option& operator=(const option&) = default;
    option& operator=(option&&) = default;

    ///////////////////
    const std::string& long_name() const noexcept { return _M_long; }
    char name() const noexcept { return _M_name; }

    tern arg() const noexcept { return _M_arg; }
    bool has_arg() const noexcept { return _M_has_arg; }

    bool once() const noexcept { return _M_once; }
    int count() const noexcept { return _M_count; }

    const std::string& description() const noexcept { return _M_desc; }

    operator bool() const noexcept { return count(); }

protected:
    std::string _M_long;
    char _M_name=0;

    tern _M_arg= false;
    bool _M_has_arg= false;

    bool _M_once= true;
    int _M_count=0;

    std::string _M_desc;
    std::function<void(const std::string&)> _M_assign;

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

    reference operator[](const std::string& long_name)
    {
        iterator ri= find(long_name);
        if(ri == end()) throw std::out_of_range("option::operator[]");
        return *ri;
    }
    const_reference operator[](const std::string& long_name) const
    {
        const_iterator ri= find(long_name);
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

    iterator erase(const std::string& long_name) { return _M_c.erase(find(long_name)); }
    iterator erase(const char name) { return _M_c.erase(find(name)); }

    ////////////////////
    iterator find(const std::string& long_name)
    {
        for(auto ri= begin(); ri!=end(); ++ri) if(ri->long_name() == long_name) return ri;
        return end();
    }
    const_iterator find(const std::string& long_name) const
    {
        for(auto ri= begin(); ri!=end(); ++ri) if(ri->long_name() == long_name) return ri;
        return end();
    }
    iterator find(const char name)
    {
        for(auto ri= begin(); ri!=end(); ++ri) if(ri->name() == name) return ri;
        return end();
    }
    const_iterator find(const char name) const
    {
        for(auto ri= begin(); ri!=end(); ++ri) if(ri->name() == name) return ri;
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
