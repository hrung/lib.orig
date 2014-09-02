///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef PAM_H
#define PAM_H

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "pam_type.h"

#include <functional>
#include <map>
#include <memory>
#include <string>

///////////////////////////////////////////////////////////////////////////////////////////////////
struct pam_handle;
struct pam_conv;
struct pam_message;
struct pam_response;

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace app
{

///////////////////////////////////////////////////////////////////////////////////////////////////
typedef std::map<std::string, std::string> environment;

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace pam
{

///////////////////////////////////////////////////////////////////////////////////////////////////
typedef pam_handle* handle;

///////////////////////////////////////////////////////////////////////////////////////////////////
typedef std::function<bool(std::string&)> user_func;
typedef std::function<bool(std::string&)> pass_func;

typedef std::function<bool(const std::string&)> info_func;
typedef std::function<bool(const std::string&)> error_func;

///////////////////////////////////////////////////////////////////////////////////////////////////
class context
{
public:
    context() = default;
    context(context&) = delete;
    context(const context&) = delete;

    context(context&& x) = default;

    context(const std::string& service, const std::string& username= std::string());
    ~context();

    context& operator=(const context&) = delete;
    context& operator=(context&& x) = default;

    ////////////////////
    pam::handle handle() const noexcept { return _M_pamh; }
    bool valid() const noexcept { return _M_pamh; }

    void set_item(item, const std::string& value);
    std::string get_item(item, bool* found= nullptr);
    void reset_item(item);

    void set_user_func(user_func x)   noexcept { _M_user=x; }
    void set_pass_func(pass_func x)   noexcept { _M_pass=x; }
    void set_info_func(info_func x)   noexcept { _M_info=x; }
    void set_error_func(error_func x) noexcept { _M_error=x; }

    void set_env(const std::string& name, const std::string& value);
    std::string get_env(const std::string& name, bool* found= nullptr);
    void reset_env(const std::string& name);
    app::environment environment();

    void authenticate();

    void open_session();
    void close_session();

private:
    pam::handle _M_pamh= nullptr;
    std::unique_ptr<pam_conv> _M_conv;

    user_func _M_user;
    pass_func _M_pass;
    info_func _M_info;
    error_func _M_error;
    static int despatch(int, const pam_message**, pam_response**, void*);

    bool _M_cred= false;
    int setcred();
    int rmcred();

    int _M_code;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // PAM_H
