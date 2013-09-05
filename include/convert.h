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
template<typename ToType, typename FromType>
inline bool convert(std::stringstream& stream, const FromType& source, ToType& value)
{
    return (stream << source) && (stream >> std::ws >> value) && (stream >> std::ws).eof();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template<typename ToType, typename FromType= std::string,
         typename std::enable_if< std::is_same<ToType, bool>::value, int >::type=0 >
ToType to(const FromType& source, bool bool_alpha= false)
{
    std::stringstream stream;
    bool value;

    stream >> (bool_alpha? std::boolalpha: std::noboolalpha);

    if(convert(stream, source, value))
        return value;
    else throw except("Conversion failed");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template<typename ToType, typename FromType= std::string,
         typename std::enable_if< !std::is_same<ToType, bool>::value &&
                                   std::is_integral<ToType>::value, int >::type=0 >
ToType to(const FromType& source, int base=0)
{
    std::stringstream stream;
    ToType value;

    stream >> std::setbase(base);

    if(convert(stream, source, value))
        return value;
    else throw except("Conversion failed");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef QT_CORE_LIB
template<typename ToType, typename FromType= std::string,
         typename std::enable_if< std::is_same<ToType, QString>::value, int >::type=0 >
ToType to(const FromType& source)
{
    std::stringstream stream;
    std::string value;

    if(convert(stream, source, value))
        return QString::fromStdString(value);
    else throw except("Conversion failed");
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
template<typename ToType= std::string, typename FromType= std::string,
#ifdef QT_CORE_LIB
         typename std::enable_if< !std::is_integral<ToType>::value && !std::is_same<ToType, QString>::value, int >::type=0 >
#else
         typename std::enable_if< !std::is_integral<ToType>::value, int >::type=0 >
#endif
ToType to(const FromType& source)
{
    std::stringstream stream;
    ToType value;

    if(convert(stream, source, value))
        return value;
    else throw except("Conversion failed");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // CONVERT_H
