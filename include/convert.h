///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef CONVERT_H
#define CONVERT_H

///////////////////////////////////////////////////////////////////////////////////////////////////
#include <except.h>

#include <type_traits>
#include <sstream>
#include <iomanip>
#include <stdexcept>

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

        if((stream << source) && (stream >> std::ws >> value) && (stream >> std::ws).eof())
            return value;
        else throw except("Conversion failed");
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
template<typename ToType, typename FromType>
struct _M_convert<ToType, FromType, typename std::enable_if< std::is_integral<ToType>::value &&
                                                            !std::is_same<ToType, bool>::value >::type>
{
    static ToType to(const FromType& source, int base=0)
    {
        std::stringstream stream;
        ToType value;

        stream >> std::setbase(base);

        if((stream << source) && (stream >> std::ws >> value) && (stream >> std::ws).eof())
            return value;
        else throw except("Conversion failed");
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
template<typename FromType>
struct _M_convert<bool, FromType, void>
{
    static bool to(const FromType& source, bool bool_alpha= true)
    {
        std::stringstream stream;
        bool value;

        stream >> (bool_alpha? std::boolalpha: std::noboolalpha);

        if((stream << source) && (stream >> std::ws >> value) && (stream >> std::ws).eof())
            return value;
        else throw except("Conversion failed");
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
        else throw except("Conversion failed");
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef QT_CORE_LIB
template<typename FromType>
struct _M_convert<QString, FromType, void>
{
    static QString to(const FromType& source)
    {
        std::stringstream stream;

        if(stream << source)
            return QString::fromStdString(stream.str());
        else throw except("Conversion failed");
    }
};
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
template<typename ToType= std::string, typename FromType= std::string>
ToType to(const FromType& source)
{
    return _M_convert<ToType, FromType>::to(source);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // CONVERT_H
