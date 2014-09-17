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
credentials::credentials(const std::string& name)
{
    errno=0;
    struct passwd* pwd= getpwnam(name.data());
    if(pwd == nullptr)
    {
        if(errno) throw errno_error();
        else throw std::runtime_error("No entry for "+ name);
    }

    username= pwd->pw_name;
    fullname= pwd->pw_gecos;
    password= pwd->pw_passwd;

    uid= pwd->pw_uid;
    gid= pwd->pw_gid;

    home= pwd->pw_dir;
    shell= pwd->pw_shell;
    if(shell.empty())
    {
        setusershell();
        shell= getusershell();
        endusershell();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void credentials::morph_into()
{
    if(initgroups(username.data(), gid)) throw errno_error();
    if(setgid(gid)) throw errno_error();
    if(setuid(uid)) throw errno_error();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}
