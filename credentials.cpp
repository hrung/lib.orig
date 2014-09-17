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
void credentials::morph_into()
{
    if(initgroups(username.data(), gid)) throw errno_error();
    if(setgid(gid)) throw errno_error();
    if(setuid(uid)) throw errno_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
credentials credentials::get(const std::string& name)
{
    errno=0;
    struct passwd* pwd= getpwnam(name.data());
    if(pwd == nullptr)
    {
        if(errno) throw errno_error();
        else throw std::runtime_error("No entry for "+ name);
    }

    credentials x;

    x.username= pwd->pw_name;
    x.fullname= pwd->pw_gecos;
    x.password= pwd->pw_passwd;

    x.uid= pwd->pw_uid;
    x.gid= pwd->pw_gid;

    x.home= pwd->pw_dir;
    x.shell= pwd->pw_shell;
    if(x.shell.empty())
    {
        setusershell();
        x.shell= getusershell();
        endusershell();
    }

    return x;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}
