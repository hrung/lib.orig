///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "slap.h"
#include "slap_error.h"
#include "slap_type.h"
#include "utility.h"

#include <memory>
#include <ldap.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace slap
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
const std::error_category& slap_category()
{
    static class slap_category instance;
    return instance;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void mod_deleter::operator()(LDAPMod* mod)
{
    if(mod)
    {
        for(char** ri= mod->mod_values; ri; ++ri) delete *ri;
        delete[] mod->mod_values;

        delete[] mod->mod_type;

        delete mod;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
slap::mod to_mod(int op, const std::string& type, const std::vector<std::string>& values)
{
    slap::mod x;

    x.reset(new LDAPMod);
    x->mod_op= op;
    x->mod_type= app::clone(type).release();
    x->mod_values= new char*[values.size()+1];

    char** rp= x->mod_values;
    for(auto ri= values.cbegin(); ri != values.cend(); ++ri, ++rp) *rp= app::clone(*ri).release();
    *rp= nullptr;

    return x;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<mod> entry::get_mod() const
{
    std::vector<mod> x;
    for(const_reference ri: _M_c) x.push_back(ri.get_mod());

    return x;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
connection::connection(const std::string& uri, bool start_tls)
{
    int err= ldap_initialize(&_M_ldap, uri.data());
    if(err != LDAP_SUCCESS) throw slap_error(err);

    int opt= LDAP_VERSION3;
    ldap_set_option(_M_ldap, LDAP_OPT_PROTOCOL_VERSION, &opt);

    if(start_tls)
    {
        err= ldap_start_tls_s(_M_ldap, nullptr, nullptr);
        if(err != LDAP_SUCCESS) throw slap_error(err);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
connection::~connection()
{
    if(_M_ldap) ldap_unbind_ext_s(_M_ldap, nullptr, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void connection::bind(const std::string& dn, const std::string& passwd)
{
    auto n= app::clone(dn), p= app::clone(passwd);

    BerValue cred;
    cred.bv_val= const_cast<char*>(p.get());
    cred.bv_len= passwd.size();

    int err= ldap_sasl_bind_s(_M_ldap, n.get(), LDAP_SASL_SIMPLE, &cred, nullptr, nullptr, nullptr);
    if(err != LDAP_SUCCESS) throw slap_error(err);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
modpp connection::get_mod(std::vector<mod>& x)
{
    modpp val(new LDAPMod*[x.size()+1]);

    LDAPMod** rp= val.get();
    for(auto ri= x.begin(); ri != x.end(); ++ri, ++rp) *rp= ri->get();
    *rp= nullptr;

    return val;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void connection::add(const entry& e)
{
    auto e_mod= e.get_mod();
    modpp mod= get_mod(e_mod);

    int err= ldap_add_ext_s(_M_ldap, e.dn().data(), mod.get(), nullptr, nullptr);
    if(err != LDAP_SUCCESS) throw slap_error(err);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void connection::remove(const std::string& dn)
{
    int err= ldap_delete_ext_s(_M_ldap, dn.data(), nullptr, nullptr);
    if(err != LDAP_SUCCESS) throw slap_error(err);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void connection::modify(const entry& e)
{
    auto e_mod= e.get_mod();
    modpp mod= get_mod(e_mod);

    int err= ldap_modify_ext_s(_M_ldap, e.dn().data(), mod.get(), nullptr, nullptr);
    if(err != LDAP_SUCCESS) throw slap_error(err);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void connection::rename(const std::string& dn, const std::string& new_rdn, bool remove_old, const std::string& new_parent)
{
    int err= ldap_rename_s(_M_ldap,
        dn.data(),
        new_rdn.size()? new_rdn.data(): nullptr,
        new_parent.size()? new_parent.data(): nullptr,
        remove_old,
        nullptr, nullptr);
    if(err != LDAP_SUCCESS) throw slap_error(err);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool connection::compare(const std::string& dn, const attribute& x)
{
    BerValue bv;
    bv.bv_val= const_cast<char*>(x[0].data());
    bv.bv_len= x[0].size();

    int err= ldap_compare_ext_s(_M_ldap, dn.data(), x.name().data(), &bv, nullptr, nullptr);
    switch(err)
    {
    case LDAP_COMPARE_TRUE:
        return true;
    case LDAP_COMPARE_FALSE:
        return false;
    }
    throw slap_error(err);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
const order_func order_none( [](const entry&, const entry&) -> bool { return true; } );

///////////////////////////////////////////////////////////////////////////////////////////////////
entries connection::search(const std::string& base,
                           scope s,
                           const std::string& filter,
                           const order_func& func,
                           const slap::names& names, bool get_value)
{
    slap::entries entries;

    std::vector<char*> _M_names;
    std::vector<std::unique_ptr<char[]>> _M_names_data;

    if(names.size())
    {
        for(const std::string& name: names)
        {
            _M_names_data.push_back(app::clone(name));
            _M_names.push_back(_M_names_data.back().get());
        }
        _M_names.push_back(nullptr);
    }

    LDAPMessage* res= nullptr;
    int err= ldap_search_ext_s(_M_ldap, base.data(), int(s), filter.data(),
                               _M_names.empty()? nullptr: _M_names.data(), !get_value,
                               nullptr, nullptr, nullptr, -1, &res);
    if(err != LDAP_SUCCESS)
    {
        ldap_msgfree(res);
        throw slap_error(err);
    }
    if(ldap_count_entries(_M_ldap, res))
    {
        for(LDAPMessage* lm= ldap_first_entry(_M_ldap, res); lm; lm= ldap_next_entry(_M_ldap, lm))
        {
            char* dn= ldap_get_dn(_M_ldap, lm);
            slap::entry entry(dn);

            BerElement* be= nullptr;
            for(char* la= ldap_first_attribute(_M_ldap, lm, &be); la; la= ldap_next_attribute(_M_ldap, lm, be))
            {
                if(berval** bv= ldap_get_values_len(_M_ldap, lm, la))
                {
                    if(int n= ldap_count_values_len(bv))
                    {
                        slap::attribute attribute(la);
                        for(int i=0; i<n; ++i)
                            attribute.insert(std::string(bv[i]->bv_val, bv[i]->bv_len));

                        entry.insert(std::move(attribute));
                    }
                    ldap_value_free_len(bv);
                }
                ldap_memfree(la);
            }

            auto rc= entries.end(), ri= rc;
            while(ri != entries.begin())
            {
                if(func(*--rc, entry)) break;
                ri= rc;
            }
            entries.insert(ri, std::move(entry));

            ber_free(be, 0);
            ldap_memfree(dn);
        }
    }
    ldap_msgfree(res);

    return entries;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}
