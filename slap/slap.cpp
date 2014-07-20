///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014 Dimitry Ishenko
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
    FUNCTION_CONTEXT(ctx);
    delete_mod();

    // create empty LDAPMod
    _M_mod= new LDAPMod;
    _M_mod->mod_op= static_cast<int>(_M_operation);
    _M_mod->mod_type= nullptr;
    _M_mod->mod_values= nullptr;

    // set mod_type
    int n= _M_name.size();
    _M_mod->mod_type= new char[n+1];
    _M_name.copy(_M_mod->mod_type, n);
    _M_mod->mod_type[n]=0;

    n= _M_values.size();
    char** val= new char*[n+1];
    for(int i=0; i<=n; ++i) val[i]=0;
    _M_mod->mod_values= val;

    for(const_reference ri: _M_values)
    {
        n= ri.size();
        *val= new char[n+1];
        ri.copy(*val, n); (*val)[n]=0;
        ++val;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void attribute::delete_mod() const
{
    FUNCTION_CONTEXT(ctx);
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
    FUNCTION_CONTEXT(ctx);
    delete_mod();

    int n= _M_attributes.size();
    LDAPMod** lm= new LDAPMod*[n+1];
    for(int i=0; i<=n; ++i) lm[i]= nullptr;
    _M_mod= lm;

    for(const_reference ri: _M_attributes)
    {
        ri.create_mod();
        *(lm++)= ri._M_mod;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void entry::delete_mod() const
{
    FUNCTION_CONTEXT(ctx);
    if(_M_mod)
    {
        for(const_reference ri: _M_attributes) ri.delete_mod();
        delete[] _M_mod;
    }
    _M_mod= nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void connection::_M_open()
{
    FUNCTION_CONTEXT(ctx);
    close();

    int err= ldap_initialize(&_M_ldap, _M_uri.data());
    if(err != LDAP_SUCCESS) throw slap_error(err);

    int opt= LDAP_VERSION3;
    ldap_set_option(_M_ldap, LDAP_OPT_PROTOCOL_VERSION, &opt);

    if(_M_start_TLS)
    {
        err= ldap_start_tls_s(_M_ldap, nullptr, nullptr);
        if(err != LDAP_SUCCESS) throw slap_error(err);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void connection::close()
{
    FUNCTION_CONTEXT(ctx);

    if(_M_ldap) ldap_unbind_ext_s(_M_ldap, nullptr, nullptr);

    _M_bind_dn.clear();
    _M_ldap= nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void connection::_M_bind()
{
    FUNCTION_CONTEXT(ctx);

    BerValue cred;
    cred.bv_val= const_cast<char*>(_M_passwd.data());
    cred.bv_len= _M_passwd.size();

    int err= ldap_sasl_bind_s(_M_ldap, _M_bind_dn.data(), LDAP_SASL_SIMPLE, &cred, nullptr, nullptr, nullptr);
    if(err != LDAP_SUCCESS) throw slap_error(err);

    _M_passwd.assign(_M_passwd.size(), 'X').clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void connection::add(const entry& e)
{
    FUNCTION_CONTEXT(ctx);
    e.create_mod();

    int err= ldap_add_ext_s(_M_ldap, e.dn().data(), e._M_mod, nullptr, nullptr);
    if(err != LDAP_SUCCESS) throw slap_error(err);

    e.delete_mod();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void connection::remove(const std::string& dn)
{
    FUNCTION_CONTEXT(ctx);

    int err= ldap_delete_ext_s(_M_ldap, dn.data(), nullptr, nullptr);
    if(err != LDAP_SUCCESS) throw slap_error(err);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void connection::modify(const entry& e)
{
    FUNCTION_CONTEXT(ctx);
    e.create_mod();

    int err= ldap_modify_ext_s(_M_ldap, e.dn().data(), e._M_mod, nullptr, nullptr);
    if(err != LDAP_SUCCESS) throw slap_error(err);

    e.delete_mod();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void connection::rename(const std::string& dn, const std::string& new_rdn, bool remove_old, const std::string& new_parent)
{
    FUNCTION_CONTEXT(ctx);

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
    FUNCTION_CONTEXT(ctx);

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
    FUNCTION_CONTEXT(ctx);
    slap::entries entries;

    std::vector<char*> _M_names;
    std::vector<std::vector<char>> _M_names_data;
    if(names.size())
    {
        for(const std::string& name: names)
        {
            int length= name.size();
            _M_names_data.push_back(std::vector<char>(length+1));

            name.copy(_M_names_data.back().data(), length);
            _M_names_data.back()[length]=0;

            _M_names.push_back(_M_names_data.back().data());
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
                            attribute.append(std::string(bv[i]->bv_val, bv[i]->bv_len));

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
