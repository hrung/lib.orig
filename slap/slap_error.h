///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef SLAP_ERROR_H
#define SLAP_ERROR_H

///////////////////////////////////////////////////////////////////////////////////////////////////
#include <system_error>
#include <string>
#include <ldap.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace slap
{

///////////////////////////////////////////////////////////////////////////////////////////////////
enum class errc
{
    success                        = LDAP_SUCCESS,

    operations_error               = LDAP_OPERATIONS_ERROR,
    protocol_error                 = LDAP_PROTOCOL_ERROR,
    timelimit_exceeded             = LDAP_TIMELIMIT_EXCEEDED,
    sizelimit_exceeded             = LDAP_SIZELIMIT_EXCEEDED,
    compare_false                  = LDAP_COMPARE_FALSE,
    compare_true                   = LDAP_COMPARE_TRUE,
    auth_method_not_supported      = LDAP_AUTH_METHOD_NOT_SUPPORTED,
    strong_auth_not_supported      = LDAP_STRONG_AUTH_NOT_SUPPORTED,
    strong_auth_required           = LDAP_STRONG_AUTH_REQUIRED,
    stronger_auth_required         = LDAP_STRONGER_AUTH_REQUIRED,
    partial_results                = LDAP_PARTIAL_RESULTS,

    referral                       = LDAP_REFERRAL,
    adminlimit_exceeded            = LDAP_ADMINLIMIT_EXCEEDED,
    unavailable_critical_extension = LDAP_UNAVAILABLE_CRITICAL_EXTENSION,
    confidentiality_required       = LDAP_CONFIDENTIALITY_REQUIRED,
    sasl_bind_in_progress          = LDAP_SASL_BIND_IN_PROGRESS,

    no_such_attribute              = LDAP_NO_SUCH_ATTRIBUTE,
    undefined_type                 = LDAP_UNDEFINED_TYPE,
    inappropriate_matching         = LDAP_INAPPROPRIATE_MATCHING,
    constraint_violation           = LDAP_CONSTRAINT_VIOLATION,
    type_or_value_exists           = LDAP_TYPE_OR_VALUE_EXISTS,
    invalid_syntax                 = LDAP_INVALID_SYNTAX,

    no_such_object                 = LDAP_NO_SUCH_OBJECT,
    alias_problem                  = LDAP_ALIAS_PROBLEM,
    invalid_dn_syntax              = LDAP_INVALID_DN_SYNTAX,
    is_leaf                        = LDAP_IS_LEAF,
    alias_deref_problem            = LDAP_ALIAS_DEREF_PROBLEM,

    proxy_authz_failure            = LDAP_X_PROXY_AUTHZ_FAILURE,
    inappropriate_auth             = LDAP_INAPPROPRIATE_AUTH,
    invalid_credentials            = LDAP_INVALID_CREDENTIALS,
    insufficient_access            = LDAP_INSUFFICIENT_ACCESS,

    busy                           = LDAP_BUSY,
    unavailable                    = LDAP_UNAVAILABLE,
    unwilling_to_perform           = LDAP_UNWILLING_TO_PERFORM,
    loop_detect                    = LDAP_LOOP_DETECT,

    naming_violation               = LDAP_NAMING_VIOLATION,
    object_class_violation         = LDAP_OBJECT_CLASS_VIOLATION,
    not_allowed_on_nonleaf         = LDAP_NOT_ALLOWED_ON_NONLEAF,
    not_allowed_on_rdn             = LDAP_NOT_ALLOWED_ON_RDN,
    already_exists                 = LDAP_ALREADY_EXISTS,
    no_object_class_mods           = LDAP_NO_OBJECT_CLASS_MODS,
    results_too_large              = LDAP_RESULTS_TOO_LARGE,
    affects_multiple_dsas          = LDAP_AFFECTS_MULTIPLE_DSAS,

    vlv_error                      = LDAP_VLV_ERROR,

    other                          = LDAP_OTHER,

    cup_resources_exhausted        = LDAP_CUP_RESOURCES_EXHAUSTED,
    cup_security_violation         = LDAP_CUP_SECURITY_VIOLATION,
    cup_invalid_data               = LDAP_CUP_INVALID_DATA,
    cup_unsupported_scheme         = LDAP_CUP_UNSUPPORTED_SCHEME,
    cup_reload_required            = LDAP_CUP_RELOAD_REQUIRED,

    cancelled                      = LDAP_CANCELLED,
    no_such_operation              = LDAP_NO_SUCH_OPERATION,
    too_late                       = LDAP_TOO_LATE,
    cannot_cancel                  = LDAP_CANNOT_CANCEL,

    assertion_failed               = LDAP_ASSERTION_FAILED,

    proxied_authorization_denied   = LDAP_PROXIED_AUTHORIZATION_DENIED,

    sync_refresh_required          = LDAP_SYNC_REFRESH_REQUIRED,

    no_operation                   = LDAP_X_NO_OPERATION,

    no_referrals_found             = LDAP_X_NO_REFERRALS_FOUND,
    cannot_chain                   = LDAP_X_CANNOT_CHAIN,

    invalidreference               = LDAP_X_INVALIDREFERENCE,

    server_down                    = LDAP_SERVER_DOWN,
    local_error                    = LDAP_LOCAL_ERROR,
    encoding_error                 = LDAP_ENCODING_ERROR,
    decoding_error                 = LDAP_DECODING_ERROR,
    timeout                        = LDAP_TIMEOUT,
    auth_unknown                   = LDAP_AUTH_UNKNOWN,
    filter_error                   = LDAP_FILTER_ERROR,
    user_cancelled                 = LDAP_USER_CANCELLED,
    param_error                    = LDAP_PARAM_ERROR,
    no_memory                      = LDAP_NO_MEMORY,
    connect_error                  = LDAP_CONNECT_ERROR,
    not_supported                  = LDAP_NOT_SUPPORTED,
    control_not_found              = LDAP_CONTROL_NOT_FOUND,
    no_results_returned            = LDAP_NO_RESULTS_RETURNED,
    more_results_to_return         = LDAP_MORE_RESULTS_TO_RETURN,
    client_loop                    = LDAP_CLIENT_LOOP,
    referral_limit_exceeded        = LDAP_REFERRAL_LIMIT_EXCEEDED,
    connecting                     = LDAP_X_CONNECTING,

    user_error                     = 0x7fff /* hopefully this is not used by LDAP */
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class slap_category: public std::error_category
{
public:
    const char* name() const noexcept override { return "ldap"; }
    std::string message(int ev) const override { return ev==int(errc::user_error)? std::string(): ldap_err2string(ev); }
};

const std::error_category& slap_category();

///////////////////////////////////////////////////////////////////////////////////////////////////
inline std::error_code make_error_code(slap::errc e)
{ return std::error_code(int(e), slap_category()); }

inline std::error_condition make_error_condition(slap::errc e)
{ return std::error_condition(int(e), slap_category()); }

///////////////////////////////////////////////////////////////////////////////////////////////////
class slap_error: public std::system_error
{
public:
    slap_error(int code): std::system_error(std::error_code(code, slap::slap_category())) { }
    slap_error(int code, const std::string& message): std::system_error(std::error_code(code, slap::slap_category()), message) { }

    slap_error(slap::errc code): std::system_error(std::error_code(static_cast<int>(code), slap::slap_category())) { }
    slap_error(slap::errc code, const std::string& message): std::system_error(std::error_code(static_cast<int>(code), slap::slap_category()), message) { }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace std
{
    template<>
    struct is_error_code_enum<slap::errc>: public true_type { };
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // SLAP_ERROR_H
