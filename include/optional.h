///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef OPTIONAL_H
#define OPTIONAL_H

///////////////////////////////////////////////////////////////////////////////////////////////////
#include <utility>

///////////////////////////////////////////////////////////////////////////////////////////////////
enum none_t { none };

///////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
struct optional
{
    typedef T value_type;

    optional(): _M_none(true) { }

    ////////////////////
    optional(const optional& x) = default;
    optional& operator=(const optional& x) = default;

    ////////////////////
    optional(optional&& x): _M_value(std::move(x._M_value)), _M_none(x._M_none) { x= none; }
    optional& operator=(optional&& x)
    {
        _M_value= std::move(x._M_value); _M_none= x._M_none;
        x= none;
        return *this;
    }

    ////////////////////
    optional(const value_type& x): _M_value(x), _M_none(false) { }
    optional& operator=(const value_type& x)
    {
        _M_value= x; _M_none= false;
        return *this;
    }

    ////////////////////
    optional(value_type&& x): _M_value(std::move(x)), _M_none(false) { }
    optional& operator=(value_type&& x)
    {
        _M_value= std::move(x); _M_none= false;
        return *this;
    }

    ////////////////////
    optional(none_t): _M_none(true) { }
    optional& operator=(none_t)
    {
        _M_value= value_type(); _M_none= true;
        return *this;
    }

    ////////////////////
    template<typename U>
    explicit optional(const optional<U>& x): _M_value(x._M_value), _M_none(x._M_none) { }

    template<typename U>
    optional& operator=(const optional<U>& x)
    {
        _M_value= x; _M_none= false;
        return *this;
    }

    ////////////////////
    template<typename U>
    explicit optional(optional<U>&& x): _M_value(std::move(x._M_value)), _M_none(x._M_none) { x= none; }

    template<typename U>
    optional& operator=(optional<U>&& x)
    {
        _M_value= x; _M_none= false;
        x= none;
        return *this;
    }

    ////////////////////
    const T& value() const { return _M_value; }
    T& value() { return _M_value; }

    operator const value_type&() const { return value(); }

    const value_type* operator->() const { return &_M_value; }
    value_type* operator->() { return &_M_value; }

    const value_type& operator*() const { return _M_value; }
    value_type& operator*() { return _M_value; }

    bool is_none() { return _M_none; }

private:
    value_type _M_value;
    bool _M_none;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T> inline bool operator==(const optional<T>& x, none_t) { return  x.is_none(); }
template<typename T> inline bool operator==(none_t, const optional<T>& x) { return  x.is_none(); }

template<typename T> inline bool operator!=(none_t, const optional<T>& x) { return !x.is_none(); }
template<typename T> inline bool operator!=(const optional<T>& x, none_t) { return !x.is_none(); }

///////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
inline bool operator==(const optional<T>& x, const optional<T>& y)
{
    return ( x==none && y==none ) || ( x!=none && y!=none && *x == *y );
}

template<typename T>
inline bool operator!=(const optional<T>& x, const optional<T>& y) { return !(x==y); }

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // OPTIONAL_H
