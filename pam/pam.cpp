///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "charpp.hpp"
#include "pam.hpp"
#include "pam_error.hpp"
#include "string.hpp"

#include <cstdlib>
#include <cstring>
#include <security/pam_appl.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace pam
{

///////////////////////////////////////////////////////////////////////////////////////////////////
const std::error_category& pam_category()
{
    static class pam_category instance;
    return instance;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int context::despatch(int num, const pam_message** msg, pam_response** resp, void* data)
{
    context* instance = static_cast<context*>(data);
    *resp = (pam_response*)calloc(num, sizeof(pam_response));

    int idx=0;
    bool success= true;

    for(; idx < num; ++idx)
    {
        (*resp)[idx].resp= nullptr;
        (*resp)[idx].resp_retcode=0;

        switch(conv(msg[idx]->msg_style))
        {
        case conv::prompt_echo_on:
            if(instance->_M_user)
            {
                std::string value;
                if( (success= instance->_M_user(msg[idx]->msg, value)) ) (*resp)[idx].resp= strdup(value.data());
            }
            break;
        case conv::prompt_echo_off:
            if(instance->_M_pass)
            {
                std::string value;
                if( (success= instance->_M_pass(msg[idx]->msg, value)) ) (*resp)[idx].resp= strdup(value.data());
            }
            break;
        case conv::error_msg:
            if(instance->_M_error) success= instance->_M_error(msg[idx]->msg);
            break;
        case conv::text_info:
            if(instance->_M_info) success= instance->_M_info(msg[idx]->msg);
            break;
        }

        if(!success) break;
    }

    if(!success)
    {
        for(; idx >= 0; --idx)
            if((*resp)[idx].resp)
            {
                free((*resp)[idx].resp);
                (*resp)[idx].resp= nullptr;
            }
        free(*resp);
        (*resp)= nullptr;
    }
    return static_cast<int>(success? errc::success: errc::conv_err);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
context::context(const std::string& service, const std::string& username)
{
    auto s= app::clone(service), u= username.size()? app::clone(username): nullptr;
    pam_conv conv= { despatch, this };

    _M_code= pam_start(s.get(), u.get(), &conv, &_M_pamh);
    if(errc(_M_code) != errc::success) throw pam_error(_M_code);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void context::set_conv()
{
    if(_M_pamh)
    {
        pam_conv conv= { despatch, this };

        _M_code= pam_set_item(_M_pamh, static_cast<int>(item::conv), &conv);
        if(errc(_M_code) != errc::success) throw item_error(_M_pamh, _M_code);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void context::close() noexcept
{
    if(_M_pamh)
    {
        pam_end(_M_pamh, _M_code);
        _M_pamh= nullptr;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
std::string context::get(pam::item item, bool* found)
{
    if(item == pam::item::conv || item == pam::item::fail_delay) throw item_error(_M_pamh, errc::bad_item);

    const void* x= nullptr;
    _M_code= pam_get_item(_M_pamh, static_cast<int>(item), &x);
    if(errc(_M_code) != errc::success) throw item_error(_M_pamh, _M_code);

    if(found) *found= x;
    return x? std::string(static_cast<const char*>(x)): std::string();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void context::insert(pam::item item, const std::string& value)
{
    if(item == pam::item::conv || item == pam::item::fail_delay) throw item_error(_M_pamh, errc::bad_item);

    _M_code= pam_set_item(_M_pamh, static_cast<int>(item), value.data());
    if(errc(_M_code) != errc::success) throw item_error(_M_pamh, _M_code);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void context::erase(pam::item item)
{
    if(item == pam::item::conv || item == pam::item::fail_delay) throw item_error(_M_pamh, errc::bad_item);

    _M_code= pam_set_item(_M_pamh, static_cast<int>(item), nullptr);
    if(errc(_M_code) != errc::success) throw item_error(_M_pamh, _M_code);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
std::string context::get(const std::string& name, bool* found)
{
    const char* x= pam_getenv(_M_pamh, name.data());

    if(found) *found= x;
    return x? std::string(x): std::string();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void context::insert(const std::string& name, const std::string& value)
{
    _M_code= pam_putenv(_M_pamh, (name+ "="+ value).data());
    if(errc(_M_code) != errc::success) throw env_error(_M_pamh, _M_code);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void context::erase(const std::string& name)
{
    _M_code= pam_putenv(_M_pamh, name.data());
    if(errc(_M_code) != errc::success) throw env_error(_M_pamh, _M_code);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
app::environ context::environ() const
{
    return app::environ::from_charpp(pam_getenvlist(_M_pamh), true);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void context::authenticate()
{
    _M_code= pam_authenticate(_M_pamh, 0);
    if(errc(_M_code) != errc::success) throw auth_error(_M_pamh, _M_code);

    _M_code= pam_acct_mgmt(_M_pamh, 0);
    if(errc(_M_code) != errc::success) throw account_error(_M_pamh, _M_code);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int context::setcred()
{
    int code= pam_setcred(_M_pamh, PAM_ESTABLISH_CRED);
    _M_cred= (errc(code) == errc::success);
    return code;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int context::rmcred()
{
    int code= static_cast<int>(errc::success);
    if(_M_cred)
    {
        code= pam_setcred(_M_pamh, PAM_DELETE_CRED);
        _M_cred= false;
    }
    return code;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void context::open_session()
{
    _M_code= setcred();
    if(errc(_M_code) != errc::success) throw cred_error(_M_pamh, _M_code);

    _M_code= pam_open_session(_M_pamh, 0);
    if(errc(_M_code) != errc::success)
    {
        rmcred();
        throw session_error(_M_pamh, _M_code);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void context::close_session()
{
    _M_code= pam_close_session(_M_pamh, 0);
    if(errc(_M_code) != errc::success)
    {
        rmcred();
        throw session_error(_M_pamh, _M_code);
    }

    _M_code= rmcred();
    if(errc(_M_code) != errc::success) throw cred_error(_M_pamh, _M_code);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void context::change_pass()
{
    _M_code= pam_chauthtok(_M_pamh, 0);
    if(errc(_M_code) != errc::success) throw pass_error(_M_pamh, _M_code);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}
