///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "credentials.hpp"
#include "errno_error.hpp"

#include <grp.h>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace app
{

///////////////////////////////////////////////////////////////////////////////////////////////////
static passwd* get_pwd(app::uid x)
{
    errno = 0;
    passwd* pwd = getpwuid(x);
    if(pwd) return pwd;

    if(errno == 0)
        throw std::runtime_error("getpwuid(): entry not found");
    else throw errno_error();
}

static passwd* get_pwd(const std::string& name)
{
    errno = 0;
    passwd* pwd = getpwnam(name.data());
    if(pwd) return pwd;

    if(errno == 0)
        throw std::runtime_error("getpwnam(): entry not found");
    else throw errno_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
static std::string get_shell(passwd* pwd)
{
    std::string sh = pwd->pw_shell;
    if(sh.empty())
    {
        setusershell();
        sh = getusershell();
        endusershell();
    }
    return sh;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
credentials::credentials(app::uid x):
    credentials(get_pwd(x))
{ }

///////////////////////////////////////////////////////////////////////////////////////////////////
credentials::credentials(const std::string& name):
    credentials(get_pwd(name))
{ }

///////////////////////////////////////////////////////////////////////////////////////////////////
credentials::credentials(passwd* pwd)
{
    _M_username = pwd->pw_name;
    _M_fullname = pwd->pw_gecos;
    _M_password = pwd->pw_passwd;

    _M_uid = pwd->pw_uid;
    _M_gid = pwd->pw_gid;

    _M_home = pwd->pw_dir;
    _M_shell = get_shell(pwd);

    int num = 0;
    getgrouplist(pwd->pw_name, pwd->pw_gid, nullptr, &num);

    _M_groups.resize(num, 0);
    getgrouplist(pwd->pw_name, pwd->pw_gid, &_M_groups[0], &num);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void credentials::morph_into()
{
    if(setgroups(_M_groups.size(), &_M_groups[0])) throw errno_error();
    if(setgid(_M_gid)) throw errno_error();
    if(setuid(_M_uid)) throw errno_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace this_user
{

///////////////////////////////////////////////////////////////////////////////////////////////////
app::uid real_uid() noexcept
{
    app::uid ruid, euid, suid;
    return getresuid(&ruid, &euid, &suid) ? invalid_uid : ruid;
}
app::gid real_gid() noexcept
{
    app::gid rgid, egid, sgid;
    return getresgid(&rgid, &egid, &sgid) ? invalid_gid : rgid;
}

app::uid effective_uid() noexcept
{
    app::uid ruid, euid, suid;
    return getresuid(&ruid, &euid, &suid) ? invalid_uid : euid;
}
app::gid effective_gid() noexcept
{
    app::gid rgid, egid, sgid;
    return getresgid(&rgid, &egid, &sgid) ? invalid_gid : egid;
}

app::uid saved_uid() noexcept
{
    app::uid ruid, euid, suid;
    return getresuid(&ruid, &euid, &suid) ? invalid_uid : suid;
}
app::gid saved_gid() noexcept
{
    app::gid rgid, egid, sgid;
    return getresgid(&rgid, &egid, &sgid) ? invalid_gid : sgid;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string username() { return get_pwd(uid())->pw_name; }
std::string fullname() { return get_pwd(uid())->pw_gecos; }
std::string password() { return get_pwd(uid())->pw_passwd; }

std::string home()     { return get_pwd(uid())->pw_dir; }
std::string shell()    { return get_shell(get_pwd(uid())); }

///////////////////////////////////////////////////////////////////////////////////////////////////
app::groups groups()
{
    app::groups groups;
    if(int num = getgroups(0, nullptr))
    {
        groups.resize(num, 0);
        if(getgroups(num, &groups[0]) == -1) throw errno_error();
    }
    return groups;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void morph_into(app::uid uid, bool perm)
{
    if(setresuid(uid, perm ? uid : invalid_uid, perm ? uid : invalid_uid)) throw errno_error();
}

void morph_into(app::uid uid, app::gid gid, bool perm)
{
    if(setresgid(gid, perm ? gid : invalid_gid, perm ? gid : invalid_gid)) throw errno_error();
    morph_into(uid, perm);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}
