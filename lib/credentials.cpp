///////////////////////////////////////////////////////////////////////////////////////////////////
#include "credentials.h"
#include "errno_error.h"

#include <memory>

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace app
{

///////////////////////////////////////////////////////////////////////////////////////////////////
static passwd* get_pwd(app::uid x)
{
    errno=0;
    passwd* pwd= getpwuid(x);
    if(pwd) return pwd;

    if(errno==0)
        throw std::runtime_error("getpwuid(): entry not found");
    else throw errno_error();
}

static passwd* get_pwd(const std::string& name)
{
    errno=0;
    passwd* pwd= getpwnam(name.data());
    if(pwd) return pwd;

    if(errno==0)
        throw std::runtime_error("getpwnam(): entry not found");
    else throw errno_error();
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
    _M_username= pwd->pw_name;
    _M_fullname= pwd->pw_gecos;
    _M_password= pwd->pw_passwd;

    _M_uid= pwd->pw_uid;
    _M_gid= pwd->pw_gid;

    _M_home= pwd->pw_dir;
    _M_shell= pwd->pw_shell;
    if(_M_shell.empty())
    {
        setusershell();
        _M_shell= getusershell();
        endusershell();
    }

    int num=0;
    getgrouplist(pwd->pw_name, pwd->pw_gid, nullptr, &num);

    std::unique_ptr<app::gid[]> buffer(new app::gid[num]);
    getgrouplist(pwd->pw_name, pwd->pw_gid, buffer.get(), &num);

    for(app::gid* gi= buffer.get(); num; ++gi, --num) _M_groups.insert(*gi);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void credentials::morph_into()
{
    if(_M_groups.size())
    {
        std::unique_ptr<app::gid[]> buffer(new app::gid[_M_groups.size()]);

        app::gid* gi= buffer.get();
        for(app::gid gid: _M_groups) *gi++= gid;

        if(setgroups(_M_groups.size(), buffer.get())) throw errno_error();
    }
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
    return getresuid(&ruid, &euid, &suid)? invalid_uid: ruid;
}
app::gid real_gid() noexcept
{
    app::gid rgid, egid, sgid;
    return getresgid(&rgid, &egid, &sgid)? invalid_gid: rgid;
}

app::uid effective_uid() noexcept
{
    app::uid ruid, euid, suid;
    return getresuid(&ruid, &euid, &suid)? invalid_uid: euid;
}
app::gid effective_gid() noexcept
{
    app::gid rgid, egid, sgid;
    return getresgid(&rgid, &egid, &sgid)? invalid_gid: egid;
}

app::uid saved_uid() noexcept
{
    app::uid ruid, euid, suid;
    return getresuid(&ruid, &euid, &suid)? invalid_uid: suid;
}
app::gid saved_gid() noexcept
{
    app::gid rgid, egid, sgid;
    return getresgid(&rgid, &egid, &sgid)? invalid_gid: sgid;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string username() { return get_pwd(uid())->pw_name; }
std::string fullname() { return get_pwd(uid())->pw_gecos; }
std::string password() { return get_pwd(uid())->pw_passwd; }

std::string home() { return get_pwd(uid())->pw_dir; }

std::string shell()
{
    std::string x= get_pwd(uid())->pw_shell;
    if(x.empty())
    {
        setusershell();
        x= getusershell();
        endusershell();
    }
    return x;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
app::groups groups()
{
    app::groups x;
    if(int num= getgroups(0, nullptr))
    {
        std::unique_ptr<app::gid[]> buffer(new app::gid[num]);
        if(getgroups(num, buffer.get()) == -1) throw errno_error();

        for(app::gid* gi= buffer.get(); num; ++gi, --num) x.insert(*gi);
    }
    return x;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void morph_into(app::uid uid, bool permanent)
{
    if(setresuid(uid, uid, permanent? uid: invalid_uid)) throw errno_error();
}

void morph_into(app::uid uid, app::gid gid, bool permanent)
{
    if(setresgid(gid, gid, permanent? gid: invalid_gid)) throw errno_error();
    if(setresuid(uid, uid, permanent? uid: invalid_uid)) throw errno_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}
