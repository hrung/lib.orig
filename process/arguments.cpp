///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "arguments.hpp"

#include <cstring>
#include <stdexcept>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace app
{

///////////////////////////////////////////////////////////////////////////////////////////////////
charpp_ptr arguments::to_charpp() const
{
    char** rp = static_cast<char**>(calloc(size() + 1, sizeof(char*)));
    if(rp == nullptr) throw std::bad_alloc();

    charpp_ptr x(rp);
    for(auto ri = cbegin(); ri != cend(); ++ri, ++rp) *rp = strdup(ri->data());

    *rp = nullptr;
    return x;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
charpp_ptr arguments::to_charpp(const std::string& prepend) const
{
    char** rp = static_cast<char**>(calloc(size() + 2, sizeof(char*)));
    if(rp == nullptr) throw std::bad_alloc();

    charpp_ptr x(rp);
    *rp++ = strdup(prepend.data());

    for(auto ri = cbegin(); ri != cend(); ++ri, ++rp) *rp = strdup(ri->data());

    *rp = nullptr;
    return x;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}
