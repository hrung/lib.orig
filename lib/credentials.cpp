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
credentials::credentials(const std::string& name)
{
    errno=0;
    get(getpwnam(name.data()));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
credentials::credentials(app::uid uid)
{
    errno=0;
    get(getpwuid(uid));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void credentials::get(passwd* pwd)
{
    if(pwd)
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
    else if(errno==0)
        throw std::runtime_error("credentials::get: entry not found");
    else throw errno_error();
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
}
