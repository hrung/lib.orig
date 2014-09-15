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
static passwd* get_pwd(const std::string& name, app::uid uid)
{
    errno=0;
    passwd* pwd= name.size()? getpwnam(name.data()): getpwuid(uid);
    if(pwd) return pwd;

    if(errno==0)
        throw std::runtime_error("get_pwd: entry not found");
    else throw errno_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
static std::string get_shell(const std::string& name, app::uid uid)
{
    std::string x= get_pwd(name, uid)->pw_shell;
    if(x.empty())
    {
        setusershell();
        x= getusershell();
        endusershell();
    }
    return x;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string credentials::username() const { return get_pwd(_M_name, _M_uid)->pw_name; }
std::string credentials::fullname() const { return get_pwd(_M_name, _M_uid)->pw_gecos; }
std::string credentials::password() const { return get_pwd(_M_name, _M_uid)->pw_passwd; }

app::uid credentials::uid() const { return get_pwd(_M_name, _M_uid)->pw_uid; }
app::uid credentials::gid() const { return get_pwd(_M_name, _M_uid)->pw_gid; }

std::string credentials::home() const { return get_pwd(_M_name, _M_uid)->pw_dir; }
std::string credentials::shell() const { return get_shell(_M_name, _M_uid); }

///////////////////////////////////////////////////////////////////////////////////////////////////
app::groups credentials::groups() const
{
    app::groups x;
    passwd* pwd= get_pwd(_M_name, _M_uid);

    int num=0;
    getgrouplist(pwd->pw_name, pwd->pw_gid, nullptr, &num);

    std::unique_ptr<app::gid[]> buffer(new app::gid[num]);
    getgrouplist(pwd->pw_name, pwd->pw_gid, buffer.get(), &num);

    for(app::gid* gi= buffer.get(); num; ++gi, --num) x.insert(*gi);
    return x;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void credentials::morph_into() const
{
    passwd* pwd= get_pwd(_M_name, _M_uid);

    if(initgroups(pwd->pw_name, pwd->pw_gid))
        throw errno_error();
    else if(setgid(pwd->pw_gid))
        throw errno_error();
    else if(setuid(pwd->pw_uid)) throw errno_error();
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
std::string username() { return get_pwd(std::string(), uid())->pw_name; }
std::string fullname() { return get_pwd(std::string(), uid())->pw_gecos; }
std::string password() { return get_pwd(std::string(), uid())->pw_passwd; }

std::string home() { return get_pwd(std::string(), uid())->pw_dir; }
std::string shell() { return get_shell(std::string(), uid()); }

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
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}
