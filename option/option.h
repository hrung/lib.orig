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
#include "except.h"
#include "convert.h"
#include "tern.h"

#include <functional>
#include <initializer_list>
#include <vector>
#include <string>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace app
{

///////////////////////////////////////////////////////////////////////////////////////////////////
constexpr auto optional= uncertain;

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
class options
{
public:
    options(std::initializer_list<option> value) { for(const_reference option: value) append(option); }
    options() = default;

    typedef std::vector<option> container_type;
    typedef typename container_type::value_type value_type;

    typedef typename container_type::reference reference;
    typedef typename container_type::const_reference const_reference;
    typedef typename container_type::pointer pointer;
    typedef typename container_type::const_pointer const_pointer;

    typedef typename container_type::size_type size_type;
    typedef typename container_type::iterator iterator;
    typedef typename container_type::const_iterator const_iterator;
    typedef typename container_type::reverse_iterator reverse_iterator;
    typedef typename container_type::const_reverse_iterator const_reverse_iterator;

    ////////////////////
    bool empty() const { return _M_options.empty(); }
    size_type size() const { return _M_options.size(); }
    void clear() { _M_options.clear(); }

    ////////////////////
    reference operator[](size_type n)
    {
        FUNCTION_CONTEXT(ctx);
        try { return _M_options.at(n); }
        catch(std::out_of_range& e) { throw out_of_range(e); }
    }
    const_reference operator[](size_type n) const
    {
        FUNCTION_CONTEXT(ctx);
        try { return _M_options.at(n); }
        catch(std::out_of_range& e) { throw out_of_range(e); }
    }

    reference operator()(const std::string& longname) { return *find(longname); }
    const_reference operator()(const std::string& longname) const { return *find(longname); }

    reference operator()(const char name) { return *find(name); }
    const_reference operator()(const char name) const { return *find(name); }

    ////////////////////
    void append(const value_type& option) { _M_options.push_back(option); }
    void append(value_type&& option) { _M_options.push_back(std::move(option)); }

    template<typename... Args>
    void emplace(Args&&... args) { _M_options.emplace_back(std::forward<Args>(args)...); }

    void remove(const std::string& longname) { _M_options.erase(find(longname)); }
    void remove(const char name) { _M_options.erase(find(name)); }

    void remove(iterator ri_0, iterator ri_1) { _M_options.erase(ri_0, ri_1); }
    void remove(iterator ri) { _M_options.erase(ri); }

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

    iterator begin() { return _M_options.begin(); }
    const_iterator begin() const { return _M_options.begin(); }

    iterator end() { return _M_options.end(); }
    const_iterator end() const { return _M_options.end(); }

    reverse_iterator rbegin() { return _M_options.rbegin(); }
    const_reverse_iterator rbegin() const { return _M_options.rbegin(); }

    reverse_iterator rend() { return _M_options.rend(); }
    const_reverse_iterator rend() const { return _M_options.rend(); }

    const_iterator cbegin() const { return _M_options.cbegin(); }
    const_iterator cend() const { return _M_options.cend(); }

    const_reverse_iterator crbegin() const { return _M_options.crbegin(); }
    const_reverse_iterator crend() const { return _M_options.crend(); }

    ////////////////////
    void parse(int argc, char* argv[], int& index);
    std::string usage();

protected:
    container_type _M_options;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // OPTION_H
