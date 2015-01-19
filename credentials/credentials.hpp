///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef CREDENTIALS_HPP
#define CREDENTIALS_HPP

///////////////////////////////////////////////////////////////////////////////////////////////////
#include <string>
#include <vector>

#include <sys/types.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
struct passwd;

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace app
{

///////////////////////////////////////////////////////////////////////////////////////////////////
typedef uid_t uid;
typedef gid_t gid;
typedef std::vector<gid> groups;

constexpr uid invalid_uid = -1;
constexpr gid invalid_gid = -1;

constexpr uid root_uid = 0;
constexpr gid root_gid = 0;

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
    credentials(passwd*);

    std::string _M_username;
    std::string _M_fullname;
    std::string _M_password;

    app::uid _M_uid = invalid_uid;
    app::gid _M_gid = invalid_gid;

    std::string _M_home;
    std::string _M_shell;

    app::groups _M_groups;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
app::uid get_uid(const std::string& name);
std::string uid_name(app::uid);

app::gid get_gid(const std::string& name);
std::string gid_name(app::gid);

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace this_user
{

///////////////////////////////////////////////////////////////////////////////////////////////////
app::uid real_uid() noexcept;
app::uid effective_uid() noexcept;
app::uid saved_uid() noexcept;

app::gid real_gid() noexcept;
app::gid effective_gid() noexcept;
app::gid saved_gid() noexcept;

inline app::uid uid() noexcept { return real_uid(); }
inline app::gid gid() noexcept { return real_gid(); }

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string username();
std::string fullname();
std::string password();

std::string home();
std::string shell();

app::groups groups();

void morph_into(app::uid, bool perm = true);
void morph_into(app::uid, app::gid, bool perm = true);

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // CREDENTIALS_HPP
