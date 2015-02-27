///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ENUM_HPP
#define ENUM_HPP

///////////////////////////////////////////////////////////////////////////////////////////////////
#include <type_traits>

///////////////////////////////////////////////////////////////////////////////////////////////////
#define DECLARE_OPERATOR(e) \
typedef typename std::underlying_type<e>::type _ ## e; \
constexpr e    operator& (e x, e y) noexcept { return static_cast<e>( static_cast<_ ## e>(y)& static_cast<_ ## e>(x) ); } \
constexpr e    operator| (e x, e y) noexcept { return static_cast<e>( static_cast<_ ## e>(y)| static_cast<_ ## e>(x) ); } \
constexpr e    operator^ (e x, e y) noexcept { return static_cast<e>( static_cast<_ ## e>(y)^ static_cast<_ ## e>(x) ); } \
constexpr e    operator~ (e x)      noexcept { return static_cast<e>(                        ~static_cast<_ ## e>(x) ); } \
constexpr bool operator==(e x, e y) noexcept { return               ( static_cast<_ ## e>(y)==static_cast<_ ## e>(x) ); } \
constexpr bool operator!=(e x, e y) noexcept { return               ( static_cast<_ ## e>(y)!=static_cast<_ ## e>(x) ); } \
constexpr bool operator&&(e x, e y) noexcept { return               ( static_cast<_ ## e>(y)& static_cast<_ ## e>(x) ); } \

/// NOTE: since scoped enumerations are not implicitly convertible to bool,
/// the following will not compile:
///
/// enum class foo { bar = 1, baz = 2 };
/// foo x = foo::bar | foo::baz;
/// if(x & foo::bar) do_something(); // clang++ : error: value of type 'foo' is not contextually convertible to 'bool'
///                                  //     g++ : error: could not convert ‘operator&(...)' from 'foo' to 'bool'
///
/// Bjarne's Printer_flags example in 8.4.1 of The C++ Programming Language, 4th ed. is incorrect.
///
/// To overcome this, we add operator&&, which does the same thing as operator&, but returns bool.
/// One would use it as follows:
///
/// if(x && foo::bar) do_something();
///
/// Those who find this unpalatable can use the following:
///
/// if( (x & foo::bar) == foo::bar ) do_something();

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // ENUM_HPP
