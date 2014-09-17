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
int no_conv_func(int, const struct pam_message**, struct pam_response** resp, void*)
{
    *resp= nullptr;
    return PAM_SUCCESS;
}

pam_conv no_conv { no_conv_func, nullptr };

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
context::context(const std::string& service, const std::string& username)
{
    _M_code= pam_start(clone(service).get(), username.size()? clone(username).get(): nullptr, &no_conv, &_M_handle);
    if(errc(_M_code) != errc::success) throw pam_error(_M_code);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
context::~context()
{
    pam_end(_M_handle, _M_code);
    _M_handle= nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void context::set_item(item x, const std::string& value)
{
    if(x == item::conv || x == item::fail_delay) throw item_error(_M_handle, errc::bad_item);

    _M_code= pam_set_item(_M_handle, static_cast<int>(x), clone(value).get());
    if(errc(_M_code) != errc::success) throw item_error(_M_handle, _M_code);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string context::get_item(item x)
{
    if(x == item::conv || x == item::fail_delay) throw item_error(_M_handle, errc::bad_item);

    const void* data= nullptr;
    _M_code= pam_get_item(_M_handle, static_cast<int>(x), &data);
    if(errc(_M_code) != errc::success) throw item_error(_M_handle, _M_code);

    std::string value;
    if(data) value= static_cast<const char*>(data);

    return value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void context::authenticate()
{
    _M_code= pam_authenticate(_M_handle, 0);
    if(errc(_M_code) != errc::success) throw auth_error(_M_handle, _M_code);

    _M_code= pam_acct_mgmt(_M_handle, 0);
    if(errc(_M_code) != errc::success) throw account_error(_M_handle, _M_code);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int context::setcred()
{
    int code= pam_setcred(_M_handle, PAM_ESTABLISH_CRED);
    _M_cred= (errc(code) == errc::success);
    return code;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int context::rmcred()
{
    int code= static_cast<int>(errc::success);
    if(_M_cred)
    {
        code= pam_setcred(_M_handle, PAM_DELETE_CRED);
        _M_cred= false;
    }
    return code;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void context::open_session()
{
    _M_code= setcred();
    if(errc(_M_code) != errc::success) throw cred_error(_M_handle, _M_code);

    _M_code= pam_open_session(_M_handle, 0);
    if(errc(_M_code) != errc::success)
    {
        rmcred();
        throw session_error(_M_handle, _M_code);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void context::close_session()
{
    _M_code= pam_close_session(_M_handle, 0);
    if(errc(_M_code) != errc::success)
    {
        rmcred();
        throw session_error(_M_handle, _M_code);
    }

    _M_code= rmcred();
    if(errc(_M_code) != errc::success) throw cred_error(_M_handle, _M_code);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}
