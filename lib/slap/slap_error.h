///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef SLAP_ERROR_H
#define SLAP_ERROR_H

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "except.h"

#include <system_error>
#include <ldap.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace slap
{

///////////////////////////////////////////////////////////////////////////////////////////////////
enum class slap_error
{
    ldap_success                    = LDAP_SUCCESS,
    ldap_operations_error           = LDAP_OPERATIONS_ERROR,
    ldap_protocol_error             = LDAP_PROTOCOL_ERROR,
    ldap_timelimit_exceeded         = LDAP_TIMELIMIT_EXCEEDED,
    ldap_sizelimit_exceeded         = LDAP_SIZELIMIT_EXCEEDED,
    ldap_compare_false              = LDAP_COMPARE_FALSE,
    ldap_compare_true               = LDAP_COMPARE_TRUE,
    ldap_strong_auth_not_supported  = LDAP_STRONG_AUTH_NOT_SUPPORTED,
    ldap_strong_auth_required       = LDAP_STRONG_AUTH_REQUIRED,

    ldap_referral                   = LDAP_REFERRAL,
    ldap_adminlimit_exceeded        = LDAP_ADMINLIMIT_EXCEEDED,
    ldap_unavailable_critical_extension= LDAP_UNAVAILABLE_CRITICAL_EXTENSION,
    ldap_confidentiality_required   = LDAP_CONFIDENTIALITY_REQUIRED,
    ldap_sasl_bind_in_progress      = LDAP_SASL_BIND_IN_PROGRESS,

    ldap_no_such_attribute          = LDAP_NO_SUCH_ATTRIBUTE,
    ldap_undefined_type             = LDAP_UNDEFINED_TYPE,
    ldap_inappropriate_matching     = LDAP_INAPPROPRIATE_MATCHING,
    ldap_constraint_violation       = LDAP_CONSTRAINT_VIOLATION,
    ldap_type_or_value_exists       = LDAP_TYPE_OR_VALUE_EXISTS,
    ldap_invalid_syntax             = LDAP_INVALID_SYNTAX,

    ldap_no_such_object             = LDAP_NO_SUCH_OBJECT,
    ldap_alias_problem              = LDAP_ALIAS_PROBLEM,
    ldap_invalid_dn_syntax          = LDAP_INVALID_DN_SYNTAX,

    ldap_alias_deref_problem        = LDAP_ALIAS_DEREF_PROBLEM,

    ldap_inappropriate_auth         = LDAP_INAPPROPRIATE_AUTH,
    ldap_invalid_credentials        = LDAP_INVALID_CREDENTIALS,
    ldap_insufficient_access        = LDAP_INSUFFICIENT_ACCESS,
    ldap_busy                       = LDAP_BUSY,
    ldap_unavailable                = LDAP_UNAVAILABLE,
    ldap_unwilling_to_perform       = LDAP_UNWILLING_TO_PERFORM,
    ldap_loop_detect                = LDAP_LOOP_DETECT,

    ldap_naming_violation           = LDAP_NAMING_VIOLATION,
    ldap_object_class_violation     = LDAP_OBJECT_CLASS_VIOLATION,
    ldap_not_allowed_on_nonleaf     = LDAP_NOT_ALLOWED_ON_NONLEAF,
    ldap_not_allowed_on_rdn         = LDAP_NOT_ALLOWED_ON_RDN,
    ldap_already_exists             = LDAP_ALREADY_EXISTS,
    ldap_no_object_class_mods       = LDAP_NO_OBJECT_CLASS_MODS,

    ldap_affects_multiple_dsas      = LDAP_AFFECTS_MULTIPLE_DSAS,

    ldap_vlv_error                  = LDAP_VLV_ERROR,

    ldap_other                      = LDAP_OTHER,

    ldap_partial_results            = LDAP_PARTIAL_RESULTS,
    ldap_is_leaf                    = LDAP_IS_LEAF,

    ldap_results_too_large          = LDAP_RESULTS_TOO_LARGE,

    ldap_cancelled                  = LDAP_CANCELLED,
    ldap_no_such_operation          = LDAP_NO_SUCH_OPERATION,
    ldap_too_late                   = LDAP_TOO_LATE,
    ldap_cannot_cancel              = LDAP_CANNOT_CANCEL,

    ldap_assertion_failed           = LDAP_ASSERTION_FAILED,
    ldap_x_assertion_failed         = LDAP_X_ASSERTION_FAILED,

    ldap_proxied_authorization_denied= LDAP_PROXIED_AUTHORIZATION_DENIED,
    ldap_x_proxy_authz_failure      = LDAP_X_PROXY_AUTHZ_FAILURE,

    ldap_sync_refresh_required      = LDAP_SYNC_REFRESH_REQUIRED,
    ldap_x_sync_refresh_required    = LDAP_X_SYNC_REFRESH_REQUIRED,

    ldap_x_no_operation             = LDAP_X_NO_OPERATION,

    ldap_cup_resources_exhausted    = LDAP_CUP_RESOURCES_EXHAUSTED,
    ldap_cup_security_violation     = LDAP_CUP_SECURITY_VIOLATION,
    ldap_cup_invalid_data           = LDAP_CUP_INVALID_DATA,
    ldap_cup_unsupported_scheme     = LDAP_CUP_UNSUPPORTED_SCHEME,
    ldap_cup_reload_required        = LDAP_CUP_RELOAD_REQUIRED,

    ldap_server_down                = LDAP_SERVER_DOWN,
    ldap_local_error                = LDAP_LOCAL_ERROR,
    ldap_encoding_error             = LDAP_ENCODING_ERROR,
    ldap_decoding_error             = LDAP_DECODING_ERROR,
    ldap_timeout                    = LDAP_TIMEOUT,
    ldap_auth_unknown               = LDAP_AUTH_UNKNOWN,
    ldap_filter_error               = LDAP_FILTER_ERROR,
    ldap_user_cancelled             = LDAP_USER_CANCELLED,
    ldap_param_error                = LDAP_PARAM_ERROR,
    ldap_no_memory                  = LDAP_NO_MEMORY,
    ldap_connect_error              = LDAP_CONNECT_ERROR,
    ldap_not_supported              = LDAP_NOT_SUPPORTED,
    ldap_control_not_found          = LDAP_CONTROL_NOT_FOUND,
    ldap_no_results_returned        = LDAP_NO_RESULTS_RETURNED,
    ldap_more_results_to_return     = LDAP_MORE_RESULTS_TO_RETURN,
    ldap_client_loop                = LDAP_CLIENT_LOOP,
    ldap_referral_limit_exceeded    = LDAP_REFERRAL_LIMIT_EXCEEDED
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class slap_category: public std::error_category
{
public:
    const char* name() const noexcept override { return "ldap"; }
    std::string message(int ev) const override { return std::string(ldap_err2string(ev)); }
};

const std::error_category& slap_category();

///////////////////////////////////////////////////////////////////////////////////////////////////
inline std::error_code make_error_code(slap_error e)
{ return std::error_code(int(e), slap_category()); }

inline std::error_condition make_error_condition(slap_error e)
{ return std::error_condition(int(e), slap_category()); }

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace std
{
    template<>
    struct is_error_code_enum<slap::slap_error>: public true_type { };
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace except
{

///////////////////////////////////////////////////////////////////////////////////////////////////
class slap_exception: public exception
{
public:
    slap_exception(int error, const std::string& message= std::string()):
        exception(message, std::error_code(error, slap::slap_category()))
    { }

    slap_exception(const std::string& file, const std::string& func, int line, int error, const std::string& message= std::string()):
        exception(file, func, line, message, std::error_code(error, slap::slap_category()))
    { }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
#define slap_except(args...) except::slap_exception(__FILE__, __FUNCTION__, __LINE__, ##args)

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // SLAP_ERROR_H
