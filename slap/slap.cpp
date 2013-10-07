///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "slap.h"
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
void attribute::create_mod() const
{
    delete_mod();

    // create empty LDAPMod
    _M_mod= new LDAPMod;
    _M_mod->mod_op= int(_M_operation);
    _M_mod->mod_type= nullptr;
    _M_mod->mod_values= nullptr;

    // set mod_type
    int n= _M_name.size()+1;
    _M_mod->mod_type= new char[n];
    _M_name.copy(_M_mod->mod_type, n);
    _M_mod->mod_type[n]= 0;

    n= _M_values.size();
    char** val= new char*[n+1];
    for(int i=0; i<=n; ++i) val[i]=0;
    _M_mod->mod_values= val;

    for(const std::string& value: _M_values)
    {
        n= value.size()+1;
        *val= new char[n];
        value.copy(*val, n); (*val)[n]=0;
        ++val;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void attribute::delete_mod() const
{
    if(_M_mod)
    {
        char** val= _M_mod->mod_values;
        while(*val)
        {
            delete[] *val;
            *val=0;
            ++val;
        }
        delete[] _M_mod->mod_values;
        _M_mod->mod_values= nullptr;

        delete[] _M_mod->mod_type;
        _M_mod->mod_type= nullptr;

        delete _M_mod;
    }
    _M_mod= nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void entry::create_mod() const
{
    delete_mod();

    int n= _M_attributes.size();
    LDAPMod** lm= new LDAPMod*[n+1];
    for(int i=0; i<=n; ++i) lm[i]= nullptr;
    _M_mod= lm;

    for(const slap::attribute& ri: _M_attributes)
    {
        ri.create_mod();
        *(lm++)= ri._M_mod;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void entry::delete_mod() const
{
    if(_M_mod)
    {
        for(const slap::attribute& ri: _M_attributes) ri.delete_mod();
        delete[] _M_mod;
    }
    _M_mod= nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void connection::open(const std::string& uri, bool start_TLS)
{
    close();

    _M_uri= uri;
    int err= ldap_initialize(&_M_ldap, _M_uri.data());
    if(err != LDAP_SUCCESS) throw slap_except(err);

    int opt= LDAP_VERSION3;
    ldap_set_option(_M_ldap, LDAP_OPT_PROTOCOL_VERSION, &opt);

    if(start_TLS)
    {
        err= ldap_start_tls_s(_M_ldap, nullptr, nullptr);
        if(err != LDAP_SUCCESS) throw slap_except(err);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void connection::close()
{
    if(_M_ldap) ldap_unbind_ext_s(_M_ldap, nullptr, nullptr);

    _M_bind_dn.clear();
    _M_ldap= nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void connection::bind(const std::string& dn, const std::string& passwd)
{
    _M_bind_dn= dn;

    BerValue cred;
    cred.bv_val= const_cast<char*>(passwd.data());
    cred.bv_len= passwd.size();

    int err= ldap_sasl_bind_s(_M_ldap, _M_bind_dn.data(), LDAP_SASL_SIMPLE, &cred, nullptr, nullptr, nullptr);
    if(err != LDAP_SUCCESS) throw slap_except(err);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void connection::add(const entry& e)
{
    e.create_mod();

    int err= ldap_add_ext_s(_M_ldap, e.dn().data(), e._M_mod, nullptr, nullptr);
    if(err != LDAP_SUCCESS) throw slap_except(err);

    e.delete_mod();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void connection::remove(const std::string& dn)
{
    int err= ldap_delete_ext_s(_M_ldap, dn.data(), nullptr, nullptr);
    if(err != LDAP_SUCCESS) throw slap_except(err);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void connection::modify(const entry& e)
{
    e.create_mod();

    int err= ldap_modify_ext_s(_M_ldap, e.dn().data(), e._M_mod, nullptr, nullptr);
    if(err != LDAP_SUCCESS) throw slap_except(err);

    e.delete_mod();
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
    if(err != LDAP_SUCCESS) throw slap_except(err);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool connection::compare(const std::string& dn, const attribute& x)
{
    BerValue bv;
    bv.bv_val= const_cast<char*>(x.value().data());
    bv.bv_len= x.value(0).size();

    int err= ldap_compare_ext_s(_M_ldap, dn.data(), x.name().data(), &bv, nullptr, nullptr);
    switch(err)
    {
    case LDAP_COMPARE_TRUE:
        return true;
    case LDAP_COMPARE_FALSE:
        return false;
    }
    throw slap_except(err);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
const order_func order_none( [](const entry&, const entry&) -> bool { return true; } );

///////////////////////////////////////////////////////////////////////////////////////////////////
entries connection::search(const std::string& base,
                           scope s,
                           const std::string& filter,
                           const order_func& func,
                           const slap::names& names, bool value)
{
    slap::entries entries;

    std::vector<char*> _M_names;
    if(names.size())
    {
        for(const std::string& name: names)
            _M_names.push_back(const_cast<char*>(name.data()));
        _M_names.push_back(nullptr);
    }

    LDAPMessage* res= nullptr;
    int err= ldap_search_ext_s(_M_ldap, base.data(), int(s), filter.data(),
                               _M_names.empty()? nullptr: &_M_names[0], !value,
                               nullptr, nullptr, nullptr, -1, &res);
    if(err != LDAP_SUCCESS)
    {
        ldap_msgfree(res);
        throw slap_except(err);
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
