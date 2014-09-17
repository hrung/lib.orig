///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef CONTAINER_H
#define CONTAINER_H

///////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
class container
{
public:
    typedef T                                           container_type;
    typedef typename container_type::value_type         value_type;
    typedef typename container_type::pointer            pointer;
    typedef typename container_type::const_pointer      const_pointer;
    typedef typename container_type::reference          reference;
    typedef typename container_type::const_reference    const_reference;

    typedef typename container_type::iterator           iterator;
    typedef typename container_type::const_iterator     const_iterator;
    typedef typename container_type::reverse_iterator   reverse_iterator;
    typedef typename container_type::const_reverse_iterator const_reverse_iterator;

    typedef typename container_type::size_type          size_type;

public:
    bool empty() const { return _M_c.empty(); }
    size_type size() const { return _M_c.size(); }
    size_type max_size() const { return _M_c.max_size(); }

    void clear() { _M_c.clear(); }

    ////////////////////
    void swap(container& x) { _M_c.swap(x._M_c); }

    ////////////////////
    iterator begin() noexcept { return _M_c.begin(); }
    const_iterator begin() const noexcept { return _M_c.begin(); }

    iterator end() noexcept { return _M_c.end(); }
    const_iterator end() const noexcept { return _M_c.end(); }

    reverse_iterator rbegin() noexcept { return _M_c.rbegin(); }
    const_reverse_iterator rbegin() const noexcept { return _M_c.rbegin(); }

    reverse_iterator rend() noexcept { return _M_c.rend(); }
    const_reverse_iterator rend() const noexcept { return _M_c.rend(); }

    const_iterator cbegin() const noexcept { return _M_c.begin(); }
    const_iterator cend() const noexcept { return _M_c.end(); }

    const_reverse_iterator crbegin() const noexcept { return _M_c.rbegin(); }
    const_reverse_iterator crend() const noexcept { return _M_c.rend(); }

    ////////////////////
    template<typename U>
    friend bool operator==(const container<U>& x, const container<U>& y)
    { return x._M_c == y._M_c; }

    template<typename U>
    friend bool operator!=(const container<U>& x, const container<U>& y)
    { return x._M_c != y._M_c; }

protected:
    container_type _M_c;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
inline void swap(container<T>& x, container<T>& y) { x.swap(y); }

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif
