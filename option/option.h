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

#include <functional>
#include <stdexcept>
#include <type_traits>
#include <vector>
#include <string>
#include <initializer_list>
#include <limits>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace app
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T, typename Alloc = typename T::allocator_type>
struct is_container: public std::false_type
{ };

template<typename T, typename Alloc>
struct is_container<std::vector<T, Alloc>, Alloc>: public std::true_type
{ };

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
class option
{
public:
    ////////////////////
    ///
    /// \param long_name    long name (eg, --foo)
    /// \param name         short name (eg, -f)
    /// \param description  description
    /// \param single       this option can only be specified once
    ///
    /// option constuctor (for options without argument)
    ///
    /// \example option("foo", 'f', "Option foo");
    /// \example option("bar", "Option bar");
    /// \example option('b', "Option baz", false);
    ///
    template<typename LongNameType, typename DescriptionType>
    option(LongNameType&& long_name, char name, DescriptionType&& description, bool single= true):
        _M_long_name(std::forward<LongNameType>(long_name)),
        _M_code(name),
        _M_description(std::forward<DescriptionType>(description)),
        _M_single(single)
    { }

    template<typename LongNameType, typename DescriptionType>
    option(LongNameType&& long_name, DescriptionType&& description, bool single= true):
        _M_long_name(std::forward<LongNameType>(long_name)),
        _M_code(generate()),
        _M_description(std::forward<DescriptionType>(description)),
        _M_single(single)
    { }

    template<typename DescriptionType>
    option(char name, DescriptionType&& description, bool single= true):
        _M_code(name),
        _M_description(std::forward<DescriptionType>(description)),
        _M_single(single)
    { }

    ////////////////////
    ///
    /// \param long_name    long name (eg, --foo)
    /// \param name         short name (eg, -f)
    /// \param description  description
    /// \param value        variable to store the value of the argument
    /// \param need_arg     whether the argument is needed (must be true or uncertain)
    ///
    /// option constuctor
    ///
    /// \note If the value is of type std::vector, the option can be
    /// specified more than once; other containers (eg, std::set, etc) can
    /// also be specified by adding specialization to is_container.
    ///
    /// \example vector<int> arg1; option("foo", 'f', "Option foo", arg1);
    /// \example bool arg2;        option("bar", "Option bar", arg2);
    /// \example string arg3;      option('b', "Option baz", arg3, optional);
    ///
    template<typename LongNameType, typename DescriptionType, typename ValueType>
    option(LongNameType&& long_name, char name, DescriptionType&& description, ValueType& value, tern need_arg= true):
        _M_long_name(std::forward<LongNameType>(long_name)),
        _M_code(name),
        _M_description(std::forward<DescriptionType>(description)),
        _M_need(need_arg? need_arg: uncertain),
        _M_single(option_assign<ValueType>::single),
        assign(std::bind(option_assign<ValueType>::assign, std::ref(value), std::placeholders::_1))
    { }

    template<typename LongNameType, typename DescriptionType, typename ValueType>
    option(LongNameType&& long_name, DescriptionType&& description, ValueType& value, tern need_arg= true):
        _M_long_name(std::forward<LongNameType>(long_name)),
        _M_code(generate()),
        _M_description(std::forward<DescriptionType>(description)),
        _M_need(need_arg? need_arg: uncertain),
        _M_single(option_assign<ValueType>::single),
        assign(std::bind(option_assign<ValueType>::assign, std::ref(value), std::placeholders::_1))
    { }

    template<typename DescriptionType, typename ValueType>
    option(char name, DescriptionType&& description, ValueType& value, tern need_arg= true):
        _M_code(name),
        _M_description(std::forward<DescriptionType>(description)),
        _M_need(need_arg? need_arg: uncertain),
        _M_single(option_assign<ValueType>::single),
        assign(std::bind(option_assign<ValueType>::assign, std::ref(value), std::placeholders::_1))
    { }

    ///////////////////
    option() = default;
    option(const option&) = default;
    option(option&&) = default;

    option& operator=(const option&) = default;
    option& operator=(option&&) = default;

    ///////////////////
    const std::string& long_name() const noexcept { return _M_long_name; }
    char name() const noexcept { return static_cast<char>(_M_code > name_max? 0: _M_code); }

    tern need_arg() const noexcept { return _M_need; }
    bool single() const noexcept { return _M_single; }

    int count() const noexcept { return _M_count; }
    bool have_arg() const noexcept { return _M_have; }

    const std::string& description() const noexcept { return _M_description; }

private:
    std::string _M_long_name;
    int _M_code;
    std::string _M_description;

    tern _M_need= false;
    bool _M_single;

    int _M_count=0;
    bool _M_have= false;

    std::function<void(const std::string&)> assign= nullptr;

    ///////////////////
    void inc_count() noexcept { ++_M_count; }
    void set_have() noexcept { _M_have= true; }

    int code() const noexcept { return _M_code; }
    bool generated() const noexcept { return _M_code > name_max; }

    static constexpr int name_max= std::numeric_limits<char>::max();
    static int generate();

    friend class options;

    ///////////////////
    template<typename T, typename = void>
    struct option_assign
    {
        static void assign(T& value, const std::string& source)
        {
            value= convert::to<T>(source);
        }
        static constexpr bool single= true;
    };

    template<typename T>
    struct option_assign<T, typename std::enable_if<is_container<T>::value>::type>
    {
        static void assign(T& value, const std::string& source)
        {
            value.insert(value.end(), convert::to<typename T::value_type>(source));
        }
        static constexpr bool single= false;
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
    reference operator()(const std::string& long_name)
    {
        iterator ri= find(long_name);
        if(ri == end()) throw std::out_of_range("option::operator()");
        return *ri;
    }
    const_reference operator()(const std::string& long_name) const
    {
        const_iterator ri= find(long_name);
        if(ri == end()) throw std::out_of_range("option::operator()");
        return *ri;
    }

    reference operator()(char name)
    {
        iterator ri= find(name);
        if(ri == end()) throw std::out_of_range("option::operator()");
        return *ri;
    }
    const_reference operator()(char name) const
    {
        const_iterator ri= find(name);
        if(ri == end()) throw std::out_of_range("option::operator()");
        return *ri;
    }

    ////////////////////
    using container::insert;
    using container::erase;

    iterator erase(const std::string& long_name) { return _M_c.erase(find(long_name)); }
    iterator erase(char name) { return _M_c.erase(find(name)); }

    ////////////////////
    iterator find(const std::string& long_name)
    {
        for(auto ri= begin(); ri!=end(); ++ri)
            if(ri->long_name() == long_name) return ri;
        return end();
    }
    const_iterator find(const std::string& long_name) const
    {
        for(auto ri= begin(); ri!=end(); ++ri)
            if(ri->long_name() == long_name) return ri;
        return end();
    }

    iterator find(char name)
    {
        for(auto ri= begin(); ri!=end(); ++ri)
            if(ri->name() == name) return ri;
        return end();
    }
    const_iterator find(char name) const
    {
        for(auto ri= begin(); ri!=end(); ++ri)
            if(ri->name() == name) return ri;
        return end();
    }

    ////////////////////
    void parse(int argc, char* argv[], int& index);
    std::string usage() const;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // OPTION_H
