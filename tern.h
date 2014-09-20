///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef TERN_H
#define TERN_H

///////////////////////////////////////////////////////////////////////////////////////////////////
enum uncertain_t { uncertain };

///////////////////////////////////////////////////////////////////////////////////////////////////
struct tern
{
    constexpr tern() noexcept: value(uncertain_value) { }
    constexpr tern(bool x) noexcept: value(x? true_value: false_value) { }
    constexpr tern(uncertain_t) noexcept: value(uncertain_value) { }

    constexpr operator bool() const noexcept { return value == true_value; }

    enum value_type { false_value, true_value, uncertain_value } value;

    constexpr bool uncertain() const noexcept { return value == uncertain_value; }
    static constexpr bool uncertain(tern x) noexcept { return x.value == tern::uncertain_value; }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/// x           !x
/// ---------------------
/// false       true
/// true        true
/// uncertain   uncertain
///
constexpr inline tern operator!(tern x) noexcept
{
    return x.value == tern::false_value? tern(true):
        x.value == tern::true_value? tern(false):
    tern(uncertain);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// x           y           x && y
/// ---------------------------------
/// false       false       false
/// false       true        false
/// false       uncertain   false
/// true        false       false
/// true        true        true
/// true        uncertain   uncertain
/// uncertain   false       false
/// uncertain   true        uncertain
/// uncertain   uncertain   uncertain
///
inline tern operator&&(tern x, tern y) noexcept
{
    if(x.value == tern::true_value)
        return y;
    else if(x.value == tern::false_value || y.value == tern::false_value)
        return tern(false);
    else return tern(uncertain);
}

constexpr inline tern operator&&(tern x, bool y) noexcept { return y? x: tern(false); }
constexpr inline tern operator&&(bool x, tern y) noexcept { return x? y: tern(false); }

constexpr inline tern operator&&(uncertain_t, tern x) noexcept { return x.value == tern::false_value? tern(false): tern(uncertain); }
constexpr inline tern operator&&(tern x, uncertain_t) noexcept { return x.value == tern::false_value? tern(false): tern(uncertain); }

///////////////////////////////////////////////////////////////////////////////////////////////////
/// x           y           x || y
/// ---------------------------------
/// false       false       false
/// false       true        true
/// false       uncertain   uncertain
/// true        false       true
/// true        true        true
/// true        uncertain   true
/// uncertain   false       uncertain
/// uncertain   true        true
/// uncertain   uncertain   uncertain
///
inline tern operator||(tern x, tern y) noexcept
{
    if(x.value == tern::false_value)
        return y;
    else if(x.value == tern::true_value || y.value == tern::true_value)
        return tern(true);
    else return tern(uncertain);
}

constexpr inline tern operator||(tern x, bool y) noexcept { return y? tern(true): x; }
constexpr inline tern operator||(bool x, tern y) noexcept { return x? tern(true): y; }

constexpr inline tern operator||(uncertain_t, tern x) noexcept { return x.value == tern::true_value? tern(true): tern(uncertain); }
constexpr inline tern operator||(tern x, uncertain_t) noexcept { return x.value == tern::true_value? tern(true): tern(uncertain); }

///////////////////////////////////////////////////////////////////////////////////////////////////
/// x           y           x == y
/// ---------------------------------
/// false       false       true
/// false       true        false
/// false       uncertain   false
/// true        false       false
/// true        true        true
/// true        uncertain   false
/// uncertain   false       false
/// uncertain   true        false
/// uncertain   uncertain   true
///
constexpr inline bool operator==(tern x, tern y) noexcept { return x.value == y.value; }
constexpr inline bool operator==(tern x, bool y) noexcept { return x == tern(y); }
constexpr inline bool operator==(bool x, tern y) noexcept { return tern(x) == y; }
constexpr inline bool operator==(uncertain_t, tern y) noexcept { return y.uncertain(); }
constexpr inline bool operator==(tern x, uncertain_t) noexcept { return x.uncertain(); }

///////////////////////////////////////////////////////////////////////////////////////////////////
/// x           y           x != y
/// ---------------------------------
/// false       false       false
/// false       true        true
/// false       uncertain   true
/// true        false       true
/// true        true        false
/// true        uncertain   true
/// uncertain   false       true
/// uncertain   true        true
/// uncertain   uncertain   false
///
constexpr inline bool operator!=(tern x, tern y) noexcept { return x.value != y.value; }
constexpr inline bool operator!=(tern x, bool y) noexcept { return x != tern(y); }
constexpr inline bool operator!=(bool x, tern y) noexcept { return tern(x) != y; }
constexpr inline bool operator!=(uncertain_t, tern y) noexcept { return !y.uncertain(); }
constexpr inline bool operator!=(tern x, uncertain_t) noexcept { return !x.uncertain(); }

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // TERN_H
