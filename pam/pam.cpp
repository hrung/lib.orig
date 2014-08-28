///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "pam.h"
#include "pam_error.h"

#include <memory>
#include <security/pam_appl.h>
#include <cstdlib>
#include <cstring>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace pam
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
const std::error_category& pam_category()
{
    static class pam_category instance;
    return instance;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
std::unique_ptr<char[]> clone(const std::string& value)
{
    std::unique_ptr<char[]> buffer(new char[value.size()+1]);

    value.copy(buffer.get(), value.size());
    buffer[value.size()]=0;

    return buffer;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int context::despatch(int num, const pam_message** msg, pam_response** resp, void* data)
{
    context* instance= static_cast<context*>(data);
    (*resp)= (pam_response*)calloc(num, sizeof(pam_response));

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
                if( (success= instance->_M_user(value)) ) (*resp)[idx].resp= strdup(value.data());
            }
            break;
        case conv::prompt_echo_off:
            if(instance->_M_pass)
            {
                std::string value;
                if( (success= instance->_M_pass(value)) ) (*resp)[idx].resp= strdup(value.data());
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
///////////////////////////////////////////////////////////////////////////////////////////////////
context::context(const std::string& service, const std::string& username)
{
    _M_conv= std::unique_ptr<pam_conv>(new pam_conv);
    _M_conv->conv= despatch;
    _M_conv->appdata_ptr= this;

    _M_code= pam_start(clone(service).get(), username.size()? clone(username).get(): nullptr, _M_conv.get(), &_M_pamh);
    if(errc(_M_code) != errc::success) throw pam_error(_M_code);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
context::~context()
{
    pam_end(_M_pamh, _M_code);
    _M_pamh= nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void context::set_item(item x, const std::string& value)
{
    if(x == item::conv || x == item::fail_delay) throw item_error(_M_pamh, errc::bad_item);

    _M_code= pam_set_item(_M_pamh, static_cast<int>(x), clone(value).get());
    if(errc(_M_code) != errc::success) throw item_error(_M_pamh, _M_code);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string context::get_item(item x)
{
    if(x == item::conv || x == item::fail_delay) throw item_error(_M_pamh, errc::bad_item);

    const void* data= nullptr;
    _M_code= pam_get_item(_M_pamh, static_cast<int>(x), &data);
    if(errc(_M_code) != errc::success) throw item_error(_M_pamh, _M_code);

    std::string value;
    if(data) value= static_cast<const char*>(data);

    return value;
}

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
}
