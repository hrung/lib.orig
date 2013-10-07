///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef FLAGS_H
#define FLAGS_H

///////////////////////////////////////////////////////////////////////////////////////////////////
template<typename Enum>
class flags
{
public:
    constexpr flags(): _M_value(0) { }
    constexpr flags(Enum x): _M_value(int(x)) { }
    constexpr flags(const flags& x): _M_value(x._M_value) { }

    ////////////////////
    bool contains(Enum x) const { return (_M_value & int(x)) == int(x) && (int(x) || !_M_value); }
    bool empty() const { return _M_value; }
    void clear() { _M_value=0; }

    constexpr operator int() const { return _M_value; }

    ////////////////////
    constexpr flags operator&(Enum x)  const { return Enum(_M_value & int(x)); }
    constexpr flags operator&(flags x) const { return Enum(_M_value & int(x)); }

    constexpr flags operator|(Enum x)  const { return Enum(_M_value | int(x)); }
    constexpr flags operator|(flags x) const { return Enum(_M_value | int(x)); }

    constexpr flags operator^(Enum x)  const { return Enum(_M_value ^ int(x)); }
    constexpr flags operator^(flags x) const { return Enum(_M_value ^ int(x)); }

    constexpr flags operator~()        const { return Enum(~_M_value); }

    ////////////////////
    flags& operator=(Enum x)   { _M_value = int(x);     return *this; }
    flags& operator=(flags x)  { _M_value = x._M_value; return *this; }

    flags& operator&=(Enum x)  { _M_value&= int(x);     return *this; }
    flags& operator&=(flags x) { _M_value&= x._M_value; return *this; }
    flags& operator&=(int x)   { _M_value&= x;          return *this; }

    flags& operator|=(Enum x)  { _M_value|= int(x);     return *this; }
    flags& operator|=(flags x) { _M_value|= x._M_value; return *this; }

    flags& operator^=(Enum x)  { _M_value^= int(x);     return *this; }
    flags& operator^=(flags x) { _M_value^= x._M_value; return *this; }

private:
    int _M_value;
};

#define ENUM_FLAGS(Enum) \
    typedef flags<Enum> Enum##_flags; \
    constexpr Enum##_flags operator&(Enum x, Enum##_flags y) { return y & x; } \
    constexpr Enum##_flags operator&(Enum x, Enum y)       { return Enum##_flags(x) & y; } \
    constexpr Enum##_flags operator|(Enum x, Enum##_flags y) { return y | x; } \
    constexpr Enum##_flags operator|(Enum x, Enum y)       { return Enum##_flags(x) | y; } \
    constexpr Enum##_flags operator^(Enum x, Enum##_flags y) { return y ^ x; } \
    constexpr Enum##_flags operator^(Enum x, Enum y)       { return Enum##_flags(x) ^ y; } \
    constexpr Enum##_flags operator~(Enum x) { return ~Enum##_flags(x); }

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // FLAGS_H
