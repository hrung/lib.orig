///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef SLAP_TYPE_HPP
#define SLAP_TYPE_HPP

///////////////////////////////////////////////////////////////////////////////////////////////////
#include <memory>
#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////////////////
struct ldapmod;
typedef ldapmod LDAPMod;

struct ldap;
typedef ldap LDAP;

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace slap
{

///////////////////////////////////////////////////////////////////////////////////////////////////
typedef LDAP* ldap;

///////////////////////////////////////////////////////////////////////////////////////////////////
struct mod_deleter
{
    void operator()(LDAPMod* mod);
};

typedef std::unique_ptr<LDAPMod, mod_deleter> mod;

slap::mod to_mod(int op, const std::string& type, const std::vector<std::string>& values);

///////////////////////////////////////////////////////////////////////////////////////////////////
typedef std::unique_ptr<LDAPMod*[]> modpp;

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // SLAP_TYPE_HPP
