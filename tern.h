///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef TERN_H
#define TERN_H

///////////////////////////////////////////////////////////////////////////////////////////////////
enum uncertain_type { uncertain };

///////////////////////////////////////////////////////////////////////////////////////////////////
struct tern
{
    tern(): value(uncertain_value) { }
    tern(bool x): value(x? true_value: false_value) { }
    tern(uncertain_type): value(uncertain_value) { }

    operator bool() const { return value == true_value; }

    enum value_type { false_value, true_value, uncertain_value } value;

    bool uncertain() const { return value == uncertain_value; }
    static bool uncertain(tern x) { return x.value == tern::uncertain_value; }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/// x           !x
/// ---------------------
/// false       true
/// true        true
/// uncertain   uncertain
///
inline tern operator!(tern x)
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
inline tern operator&&(tern x, tern y)
{
    if(x.value == tern::true_value)
        return y;
    else if(x.value == tern::false_value || y.value == tern::false_value)
        return tern(false);
    else return tern(uncertain);
}

inline tern operator&&(tern x, bool y) { return y? x: tern(false); }
inline tern operator&&(bool x, tern y) { return x? y: tern(false); }

inline tern operator&&(uncertain_type, tern x) { return x.value == tern::false_value? tern(false): tern(uncertain); }
inline tern operator&&(tern x, uncertain_type) { return x.value == tern::false_value? tern(false): tern(uncertain); }

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
inline tern operator||(tern x, tern y)
{
    if(x.value == tern::false_value)
        return y;
    else if(x.value == tern::true_value || y.value == tern::true_value)
        return tern(true);
    else return tern(uncertain);
}

inline tern operator||(tern x, bool y) { return y? tern(true): x; }
inline tern operator||(bool x, tern y) { return x? tern(true): y; }

inline tern operator||(uncertain_type, tern x) { return x.value == tern::true_value? tern(true): tern(uncertain); }
inline tern operator||(tern x, uncertain_type) { return x.value == tern::true_value? tern(true): tern(uncertain); }

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
inline bool operator==(tern x, tern y) { return x.value == y.value; }
inline bool operator==(tern x, bool y) { return x == tern(y); }
inline bool operator==(bool x, tern y) { return tern(x) == y; }
inline bool operator==(uncertain_type, tern y) { return y.uncertain(); }
inline bool operator==(tern x, uncertain_type) { return x.uncertain(); }

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
inline bool operator!=(tern x, tern y) { return x.value != y.value; }
inline bool operator!=(tern x, bool y) { return x != tern(y); }
inline bool operator!=(bool x, tern y) { return tern(x) != y; }
inline bool operator!=(uncertain_type, tern y) { return !y.uncertain(); }
inline bool operator!=(tern x, uncertain_type) { return !x.uncertain(); }

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // TERN_H
