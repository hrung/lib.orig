///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef COMMON_H
#define COMMON_H

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "except.h"

#include <vector>
#include <string>

#include <unistd.h>
#include <limits.h>
#include <syslog.h>
#include <libgen.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
enum class level
{
    emerg   = LOG_EMERG,
    alert   = LOG_ALERT,
    critical= LOG_CRIT,
    error   = LOG_ERR,
    warning = LOG_WARNING,
    notice  = LOG_NOTICE,
    info    = LOG_INFO,
    debug   = LOG_DEBUG
};

void show_message(const std::string& what, level= level::info, bool force= false);
void show_message(const except::exception&, bool force= false);

///////////////////////////////////////////////////////////////////////////////////////////////////
inline std::string hostname()
{
    char buffer[HOST_NAME_MAX+1];
    if(gethostname(buffer, HOST_NAME_MAX)) throw system_except();

    return std::string(buffer);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
inline std::string filename(const std::string& path)
{
    return std::string(basename(const_cast<char*>( path.data() )));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool system(const std::string& command);

typedef std::vector<std::string> arguments;
pid_t execute(const std::string& name, const arguments& args= arguments());

inline void reboot() { execute("/sbin/reboot"); }

///////////////////////////////////////////////////////////////////////////////////////////////////
extern std::string prog_name;

extern bool verbose;
extern bool logging;

extern bool exiting;
extern int  exit_code;

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif
