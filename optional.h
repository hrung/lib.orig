///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014 Dimitry Ishenko
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

    optional() = default;

    ////////////////////
    template<typename U>
    explicit optional(const optional<U>& x) { swap(x); }

    template<typename U>
    optional& operator=(const optional<U>& x)
    {
        swap(x);
        return (*this);
    }

    ////////////////////
    template<typename U>
    explicit optional(optional<U>&& x) { swap(x); }

    template<typename U>
    optional& operator=(optional<U>&& x)
    {
        swap(x);
        return (*this);
    }

    ////////////////////
    optional(const value_type& x) { copy(x); }

    optional& operator=(const value_type& x)
    {
        copy(x);
        return (*this);
    }

    ////////////////////
    optional(value_type&& x) { swap(x); }
    optional& operator=(value_type&& x)
    {
        swap(x);
        return (*this);
    }

    ////////////////////
    optional(none_t) { clear(); }
    optional& operator=(none_t)
    {
        clear();
        return (*this);
    }

    ////////////////////
    template<typename U>
    void swap(optional<U>& x)
    {
        std::swap(_M_value, x._M_value);
        std::swap(_M_none, x._M_none);
    }
    void swap(value_type& x) { std::swap(_M_value, x); }

    template<typename U>
    void copy(optional<U>& x)
    {
        _M_value= x._M_value;
        _M_none= x._M_none;
    }
    void copy(const value_type& x)
    {
        _M_value= x;
        _M_none= false;
    }
    void clear()
    {
        _M_value= value_type();
        _M_none= true;
    }

    ////////////////////
    const value_type* operator->() const noexcept { return &_M_value; }
    value_type* operator->() noexcept { return &_M_value; }

    const value_type& operator*() const noexcept { return _M_value; }
    value_type& operator*() noexcept { return _M_value; }

    bool is_none() const noexcept { return _M_none; }

private:
    value_type _M_value;
    bool _M_none= true;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T> inline bool operator==(const optional<T>& x, none_t) noexcept { return  x.is_none(); }
template<typename T> inline bool operator==(none_t, const optional<T>& x) noexcept { return  x.is_none(); }

template<typename T> inline bool operator!=(none_t, const optional<T>& x) noexcept { return !x.is_none(); }
template<typename T> inline bool operator!=(const optional<T>& x, none_t) noexcept { return !x.is_none(); }

///////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
inline bool operator==(const optional<T>& x, const optional<T>& y)
{
    return ( x==none && y==none ) || ( x!=none && y!=none && *x == *y );
}

template<typename T>
inline bool operator!=(const optional<T>& x, const optional<T>& y) { return !(x==y); }

///////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T> inline bool operator==(const optional<T>& x, const T& y) { return x==optional<T>(y); }
template<typename T> inline bool operator==(const T& x, const optional<T>& y) { return optional<T>(x)==y; }

template<typename T> inline bool operator!=(const T& x, const optional<T>& y) { return optional<T>(x)!=y; }
template<typename T> inline bool operator!=(const optional<T>& x, const T& y) { return x!=optional<T>(y); }

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // OPTIONAL_H
