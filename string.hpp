///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef STRING_HPP
#define STRING_HPP

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "enum.hpp"

#include <algorithm>
#include <cctype>
#include <memory>
#include <string>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace app
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief clone
///
/// Returns smart char* pointer with a copy of the contents of the std::string
/// for temporary use in functions requiring char*.
///
inline std::unique_ptr<char[]> clone(const std::string& value)
{
    std::unique_ptr<char[]> buffer(new char[value.size() + 1]);

    value.copy(buffer.get(), value.size());
    buffer[value.size()] = '\0';

    return buffer;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
enum class whence
{
    head = 1,
    tail = 2,
    both = whence::head | whence::tail,
};
DECLARE_OPERATOR(whence)

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief  trim leading and/or trailing spaces from the string
/// \param  source  string to trim
/// \param  whence  parts of the string to trim
/// \return trimmed string
///
/// This function is written as a template so that it works on both
/// lvalue and rvalue references, thus avoiding unneeded copying.
///
/// Cf: reference collapsing rules, universal references
///
template<typename T>
inline std::string trim(T&& source, app::whence whence = app::whence::both)
{
    static_assert(std::is_convertible<T, std::string>::value, "Typename T must be convertible to std::string");

    ////////////////////
    /// std::find_if_not will not work directly with std::isspace,
    /// as there are 2 overloaded versions of it.
    ///
    auto isspace = [](char c) -> bool
    {
        // convert c to unsigned char to make sure
        // it's non-negative (ISO C requirement)
        return std::isspace(static_cast<unsigned char>(c));
    };
    std::string copy(std::forward<std::string>(source));

    if(whence && app::whence::head) copy.erase(copy.begin(), std::find_if_not(copy.begin(), copy.end(), isspace));
    if(whence && app::whence::tail) copy.erase(std::find_if_not(copy.rbegin(), copy.rend(), isspace).base(), copy.end());

    return copy;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // STRING_HPP
