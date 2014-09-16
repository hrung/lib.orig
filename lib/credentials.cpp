///////////////////////////////////////////////////////////////////////////////////////////////////
#include "credentials.h"
#include "errno_error.h"

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
static std::string get_shell(passwd* pwd)
{
    std::string x= pwd->pw_shell;
    if(x.empty())
    {
        setusershell();
        x= getusershell();
        endusershell();
    }
    return x;
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
    _M_shell= get_shell(pwd);

    _M_num=0;
    getgrouplist(pwd->pw_name, pwd->pw_gid, nullptr, &_M_num);

    _M_group.reset(new app::gid[_M_num]);
    getgrouplist(pwd->pw_name, pwd->pw_gid, _M_group.get(), &_M_num);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
app::groups credentials::groups() const
{
    app::groups x;

    int num= _M_num;
    for(app::gid* gi= _M_group.get(); num; --num, ++gi) x.insert(*gi);

    return x;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void credentials::morph_into()
{
    if(setgroups(_M_num, _M_group.get())) throw errno_error();
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
std::string shell() { return get_shell(get_pwd(uid())); }

///////////////////////////////////////////////////////////////////////////////////////////////////
app::groups groups()
{
    app::groups x;
    if(int num= getgroups(0, nullptr))
    {
        std::unique_ptr<app::gid[]> buffer(new app::gid[num]);
        if(getgroups(num, buffer.get()) == -1) throw errno_error();

        for(app::gid* gi= buffer.get(); num; --num, ++gi) x.insert(*gi);
    }
    return x;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void morph_into(app::uid uid, bool perm)
{
    if(setresuid(uid, perm? uid: invalid_uid, perm? uid: invalid_uid)) throw errno_error();
}

void morph_into(app::uid uid, app::gid gid, bool perm)
{
    if(setresgid(gid, perm? gid: invalid_gid, perm? gid: invalid_gid)) throw errno_error();
    morph_into(uid, perm);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}
