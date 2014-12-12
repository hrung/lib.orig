///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef PAM_ERROR_HPP
#define PAM_ERROR_HPP

///////////////////////////////////////////////////////////////////////////////////////////////////
#include <system_error>
#include <security/pam_appl.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
struct pam_handle;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace pam
{

///////////////////////////////////////////////////////////////////////////////////////////////////
enum class errc
{
    success               = PAM_SUCCESS,
    open_err              = PAM_OPEN_ERR,
    symbol_err            = PAM_SYMBOL_ERR,
    service_err           = PAM_SERVICE_ERR,
    system_err            = PAM_SYSTEM_ERR,
    buf_err               = PAM_BUF_ERR,
    perm_denied           = PAM_PERM_DENIED,
    auth_err              = PAM_AUTH_ERR,
    cred_insufficient     = PAM_CRED_INSUFFICIENT,
    authinfo_unavail      = PAM_AUTHINFO_UNAVAIL,
    user_unknown          = PAM_USER_UNKNOWN,
    maxtries              = PAM_MAXTRIES,
    new_authtok_reqd      = PAM_NEW_AUTHTOK_REQD,
    acct_expired          = PAM_ACCT_EXPIRED,
    session_err           = PAM_SESSION_ERR,
    cred_unavail          = PAM_CRED_UNAVAIL,
    cred_expired          = PAM_CRED_EXPIRED,
    cred_err              = PAM_CRED_ERR,
    no_module_data        = PAM_NO_MODULE_DATA,
    conv_err              = PAM_CONV_ERR,
    authtok_err           = PAM_AUTHTOK_ERR,
    authtok_recovery_err  = PAM_AUTHTOK_RECOVERY_ERR,
    authtok_lock_busy     = PAM_AUTHTOK_LOCK_BUSY,
    authtok_disable_aging = PAM_AUTHTOK_DISABLE_AGING,
    try_again             = PAM_TRY_AGAIN,
    ignore                = PAM_IGNORE,
    abort                 = PAM_ABORT,
    authtok_expired       = PAM_AUTHTOK_EXPIRED,
    module_unknown        = PAM_MODULE_UNKNOWN,
    bad_item              = PAM_BAD_ITEM,
    conv_again            = PAM_CONV_AGAIN,
    incomplete            = PAM_INCOMPLETE
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class pam_category: public std::error_category
{
    const char* name() const noexcept override { return "pam"; }
    std::string message(int ev) const override { return pam_strerror(nullptr, ev); }
};

const std::error_category& pam_category();

///////////////////////////////////////////////////////////////////////////////////////////////////
inline std::error_code make_error_code(pam::errc e)
{ return std::error_code(static_cast<int>(e), pam_category()); }

inline std::error_condition make_error_condition(pam::errc e)
{ return std::error_condition(static_cast<int>(e), pam_category()); }

///////////////////////////////////////////////////////////////////////////////////////////////////
class pam_error: public std::system_error
{
public:
    pam_error(int code): std::system_error(std::error_code(code, pam_category())) { }
    pam_error(int code, const std::string& message): std::system_error(std::error_code(code, pam_category()), message) { }

    pam_error(errc code): std::system_error(std::error_code(static_cast<int>(code), pam_category())) { }
    pam_error(errc code, const std::string& message): std::system_error(std::error_code(static_cast<int>(code), pam_category()), message) { }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
typedef pam_handle* handle;

///////////////////////////////////////////////////////////////////////////////////////////////////
class pamh_error: public pam_error
{
public:
    pamh_error(pam::handle pamh, int code): pam_error(code), _M_pamh(pamh) { }
    pamh_error(pam::handle pamh, int code, const std::string& message): pam_error(code, message), _M_pamh(pamh) { }

    pamh_error(pam::handle pamh, errc code): pam_error(code), _M_pamh(pamh) { }
    pamh_error(pam::handle pamh, errc code, const std::string& message): pam_error(code, message), _M_pamh(pamh) { }

    pam::handle handle() const { return _M_pamh; }

private:
    pam::handle _M_pamh;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class item_error: public pamh_error
{
public:
    using pamh_error::pamh_error;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class env_error: public pamh_error
{
public:
    using pamh_error::pamh_error;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class auth_error: public pamh_error
{
public:
    using pamh_error::pamh_error;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class account_error: public pamh_error
{
public:
    using pamh_error::pamh_error;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class cred_error: public pamh_error
{
public:
    using pamh_error::pamh_error;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class session_error: public pamh_error
{
public:
    using pamh_error::pamh_error;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class pass_error: public pamh_error
{
public:
    using pamh_error::pamh_error;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace std
{
    template<>
    struct is_error_code_enum<pam::errc>: public true_type { };
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // PAM_ERROR_H

