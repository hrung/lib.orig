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
#include "except.h"

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
        return source;
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
using enable_if_integer = typename std::enable_if< std::is_integral<T>::value &&
                                                  !std::is_same<T, bool>::value >::type;

template<typename ToType, typename FromType>
struct _M_convert<ToType, FromType, enable_if_integer<ToType>>
{
    static ToType to(const FromType& source, int base=0)
    {
        std::stringstream stream;
        ToType value;

        stream >> std::setbase(base);

        if((stream << source) && (stream >> std::ws >> value) && (stream >> std::ws).eof())
            return value;
        else throw invalid_argument("Conversion failed");
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
        else throw invalid_argument("Conversion failed");
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
        else throw invalid_argument("Conversion failed");
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
    FUNCTION_CONTEXT(ctx);
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
    FUNCTION_CONTEXT(ctx);
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
template<typename ToType= int, typename FromType, enable_if_integer<ToType>* = nullptr>
ToType to(const FromType& source, int base)
{
    FUNCTION_CONTEXT(ctx);
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
template<typename FromType>
bool to_bool(const FromType& source, bool text)
{
    FUNCTION_CONTEXT(ctx);
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
template<typename ViaType= int, typename FromType, enable_if_integer<ViaType>* = nullptr>
std::string to_oct(const FromType& source)
{
    FUNCTION_CONTEXT(ctx);
    std::stringstream stream;

    if(stream << std::oct << to<ViaType>(source))
        return stream.str();
    else throw invalid_argument("Conversion failed");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief to_hex
/// \param source   value to be converted
/// \return         hexadecimal representation of value as std::string
///
/// Convert source from FromType to its hexadecimal representation as std::string.
///
template<typename ViaType= int, typename FromType, enable_if_integer<ViaType>* = nullptr>
std::string to_hex(const FromType& source)
{
    FUNCTION_CONTEXT(ctx);
    std::stringstream stream;

    if(stream << std::hex << to<ViaType>(source))
        return stream.str();
    else throw invalid_argument("Conversion failed");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // CONVERT_H
