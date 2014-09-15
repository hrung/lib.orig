///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef CREDENTIALS_H
#define CREDENTIALS_H

///////////////////////////////////////////////////////////////////////////////////////////////////
#include <string>
#include <set>
#include <sys/types.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
struct passwd;

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace app
{

///////////////////////////////////////////////////////////////////////////////////////////////////
typedef uid_t uid;
typedef gid_t gid;
typedef std::set<gid> groups;

constexpr uid invalid_uid= -1;
constexpr gid invalid_gid= -1;

///////////////////////////////////////////////////////////////////////////////////////////////////
class credentials
{
public:
    credentials() = default;
    credentials(const credentials&) = default;
    credentials(credentials&&) = default;

    credentials& operator=(const credentials&) = default;
    credentials& operator=(credentials&&) = default;

    explicit credentials(app::uid);
    explicit credentials(const std::string& name);

    const std::string& username() const noexcept { return _M_username; }
    const std::string& fullname() const noexcept { return _M_fullname; }
    const std::string& password() const noexcept { return _M_password; }

    app::uid uid() const noexcept { return _M_uid; }
    app::uid gid() const noexcept { return _M_uid; }

    const std::string& home() const noexcept { return _M_home; }
    const std::string& shell() const noexcept { return _M_shell; }
    const app::groups& groups() const noexcept { return _M_groups; }

    void morph_into();

private:
    std::string _M_username;
    std::string _M_fullname;
    std::string _M_password;

    app::uid _M_uid= invalid_uid;
    app::gid _M_gid= invalid_gid;

    std::string _M_home;
    std::string _M_shell;

    app::groups _M_groups;

    void get(passwd*);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // CREDENTIALS_H
