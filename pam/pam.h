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
#include "process/environ.h"
#include "pam_type.h"

#include <functional>
#include <string>
#include <utility>

///////////////////////////////////////////////////////////////////////////////////////////////////
struct pam_handle;
struct pam_message;
struct pam_response;

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace pam
{

///////////////////////////////////////////////////////////////////////////////////////////////////
typedef pam_handle* handle;

///////////////////////////////////////////////////////////////////////////////////////////////////
typedef std::function<bool(const std::string&, std::string&)> user_func;
typedef std::function<bool(const std::string&, std::string&)> pass_func;

typedef std::function<bool(const std::string&)> info_func;
typedef std::function<bool(const std::string&)> error_func;

///////////////////////////////////////////////////////////////////////////////////////////////////
class context
{
public:
    context() = default;
    context(const context&) = delete;
    context(context&& x) { swap(x); }

    context(const std::string& service, const std::string& username= std::string());
    ~context();

    context& operator=(const context&) = delete;
    context& operator=(context&& x)
    {
        swap(x);
        return (*this);
    }

    void swap(context& x)
    {
        std::swap(_M_pamh, x._M_pamh );
        std::swap(_M_user, x._M_user );
        std::swap(_M_pass, x._M_pass );
        std::swap(_M_info, x._M_info );
        std::swap(_M_error,x._M_error);
        std::swap(_M_cred, x._M_cred );
        std::swap(_M_code, x._M_code );

        set_conv();
        x.set_conv();
    }

    ////////////////////
    pam::handle handle() const noexcept { return _M_pamh; }
    bool valid() const noexcept { return _M_pamh; }

    std::string get(pam::item, bool* found= nullptr);
    void insert(pam::item, const std::string& value);
    void erase(pam::item);

    void set_user_func(user_func x)   noexcept { _M_user=x; }
    void set_pass_func(pass_func x)   noexcept { _M_pass=x; }
    void set_info_func(info_func x)   noexcept { _M_info=x; }
    void set_error_func(error_func x) noexcept { _M_error=x; }

    std::string get(const std::string& name, bool* found= nullptr);
    void insert(const std::string& name, const std::string& value);
    void erase(const std::string& name);

    app::environ environ() const;

    void authenticate();

    void open_session();
    void close_session();

    void change_pass();

private:
    pam::handle _M_pamh= nullptr;

    user_func _M_user;
    pass_func _M_pass;
    info_func _M_info;
    error_func _M_error;
    static int despatch(int, const pam_message**, pam_response**, void*);

    void set_conv();

    bool _M_cred= false;
    int setcred();
    int rmcred();

    int _M_code;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // PAM_H
