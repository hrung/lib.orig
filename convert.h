///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef CONVERT_H
#define CONVERT_H

///////////////////////////////////////////////////////////////////////////////////////////////////
#include <stdexcept>
#include <type_traits>
#include <sstream>
#include <iomanip>

#ifdef QT_CORE_LIB
#  include <QString>
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace convert
{

///////////////////////////////////////////////////////////////////////////////////////////////////
template<typename ToType, typename FromType, typename= void>
struct _M_convert
{
    static ToType to(const FromType& source)
    {
        std::stringstream stream;
        ToType value;

        if((stream << source) && (stream >> value) && stream.eof())
            return value;
        else throw std::invalid_argument("Conversion failed");
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
using enable_if_int = typename std::enable_if< std::is_integral<T>::value &&
                                              !std::is_same<T, bool>::value >::type;

template<typename ToType, typename FromType>
struct _M_convert<ToType, FromType, enable_if_int<ToType>>
{
    static ToType to(const FromType& source, int base=0)
    {
        std::stringstream stream;
        ToType value;

        stream >> std::setbase(base);

        if((stream << source) && (stream >> std::ws >> value) && (stream >> std::ws).eof())
            return value;
        else throw std::invalid_argument("Conversion failed");
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
template<typename FromType>
struct _M_convert<bool, FromType, void>
{
    static bool to(const FromType& source, bool text= false)
    {
        std::stringstream stream;
        bool value;

        stream >> (text? std::boolalpha: std::noboolalpha);

        if((stream << source) && (stream >> std::ws >> value) && (stream >> std::ws).eof())
            return value;
        else throw std::invalid_argument("Conversion failed");
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
template<typename FromType>
struct _M_convert<std::string, FromType, void>
{
    static std::string to(const FromType& source)
    {
        std::stringstream stream;

        if(stream << source)
            return stream.str();
        else throw std::invalid_argument("Conversion failed");
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef QT_CORE_LIB
template<typename FromType>
struct _M_convert<QString, FromType, void>
{
    static QString to(const FromType& source)
    {
        return QString::fromStdString(_M_convert<std::string, FromType>::to(source));
    }
};
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief to
/// \param source   value to be converted
/// \return         value converted to ToType
///
/// Convert source from FromType to ToType.
///
/// \example        auto n= convert::to<int>("123");
/// \example        std::string s(convert::to(567));
///
template<typename ToType= std::string, typename FromType>
ToType to(const FromType& source)
{
    return _M_convert<ToType, FromType>::to(source);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief to
/// \param source   QString to be converted
/// \return         QString converted to ToType (via std::string)
///
/// Convert source from QString to ToType.
///
#ifdef QT_CORE_LIB
template<typename ToType= std::string>
ToType to(const QString& source)
{
    return _M_convert<ToType, std::string>::to(source.toStdString());
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief to
/// \param source   value to be converted
/// \param base     radix (must be one of 0, 8, 10 or 16)
/// \return         value converted to integer type
///
/// Convert source from FromType to integer type.
///
/// \example        auto n= convert::to<long>("deadbeef", 16);
///
template<typename ToType= int, typename FromType, enable_if_int<ToType>* = nullptr>
ToType to(const FromType& source, int base)
{
    return _M_convert<ToType, FromType>::to(source, base);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief to_bool
/// \param source   value to be converted
/// \param text     specify whether value is textual (true or false) or numeric (0 or 1)
/// \return         value converted to bool
///
/// Convert source from FromType to bool.
///
/// \example        bool b= convert::to_bool("false", true);
///
template<typename FromType>
bool to_bool(const FromType& source, bool text)
{
    return _M_convert<bool, FromType>::to(source, text);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief to_oct
/// \param source   value to be converted
/// \return         octal representation of value as std::string
///
/// Convert source from FromType to its octal representation as std::string.
///
/// \example        std::string o= convert::to_oct<long>(1234567890);
/// \example        std::string o= convert::to_oct("0x123");
///
template<typename ViaType= int, typename FromType, enable_if_int<ViaType>* = nullptr>
std::string to_oct(const FromType& source)
{
    std::stringstream stream;

    if(stream << std::oct << to<ViaType>(source))
        return stream.str();
    else throw std::invalid_argument("Conversion failed");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief to_hex
/// \param source   value to be converted
/// \return         hexadecimal representation of value as std::string
///
/// Convert source from FromType to its hexadecimal representation as std::string.
///
/// \example        std::string h= convert::to_hex<long>(1234567890);
/// \example        std::string h= convert::to_hex("0123");
///
template<typename ViaType= int, typename FromType, enable_if_int<ViaType>* = nullptr>
std::string to_hex(const FromType& source)
{
    std::stringstream stream;

    if(stream << std::hex << to<ViaType>(source))
        return stream.str();
    else throw std::invalid_argument("Conversion failed");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // CONVERT_H
