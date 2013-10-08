///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2013 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "common.h"
#include "stream.h"

#include <fstream>

#include <memory.h>
#include <signal.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
const std::string program()
{
    std::string name;

    std::ifstream stream("/proc/self/cmdline");
    if(stream)
    {
        std::getline(stream, name);
        name.erase(name.find(char(0)));

        name= basename(name.data());
    }
    return name;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
const std::string version() { return std::string() << major << '.' << minor; }

bool verbose= false;
bool logging= false;

bool exiting= false;
int  exit_code=0;

///////////////////////////////////////////////////////////////////////////////////////////////////
struct action
{
    action()
    {
        struct sigaction sa;
        memset(&sa, 0, sizeof(sa));

        sa.sa_handler= func;
        sa.sa_flags= SA_RESTART;

        sigaction(SIGINT, &sa, NULL);
        sigaction(SIGTERM, &sa, NULL);
    }

    static void func(int) { exiting= true; }
}
_M_action;
