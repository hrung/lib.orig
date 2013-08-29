///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef CONVERT_H
#define CONVERT_H

///////////////////////////////////////////////////////////////////////////////////////////////////
#include <except.h>

#include <type_traits>
#include <sstream>
#include <iomanip>
#include <stdexcept>

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
template<typename ToType= std::string, typename FromType= std::string,
         typename std::enable_if< !std::is_same<ToType, bool>::value &&
                                  !std::is_integral<ToType>::value, int >::type=0 >
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
