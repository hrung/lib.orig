///////////////////////////////////////////////////////////////////////////////////////////////////
#include "x11.h"
#include <X11/Xlib.h>

#include <sys/types.h>
#include <unistd.h>

#include <signal.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace X11
{

///////////////////////////////////////////////////////////////////////////////////////////////////
Server::Server(QString name, QObject* parent): QObject(parent),
    _M_name(name),
    _M_path("/usr/bin/X"),
    _M_auth("/run/camel.auth"),
    _M_args(QStringList() << "-br" <<  "-novtswitch" << "-nolisten" << "tcp" << "-quiet")
{ }

///////////////////////////////////////////////////////////////////////////////////////////////////
Server::~Server()
{
    stop();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool Server::set_name(const QString& x)
{
    if(running()) return false;
    _M_name=x;
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool Server::set_path(const QString& x)
{
    if(running()) return false;
    _M_path=x;
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool Server::set_auth(const QString& x)
{
    if(running()) return false;
    _M_auth=x;
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool Server::set_args(const QStringList& x)
{
    if(running()) return false;
    _M_args= x;
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool Server::wait_for_started(int msec)
{
    if(_M_process.state() == QProcess::NotRunning)
        return false;
    else return _M_process.waitForStarted(msec);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool Server::wait_for_stopped(int msec)
{
    if(_M_process.state() == QProcess::NotRunning)
        return true;
    else return _M_process.waitForFinished(msec);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool Server::start()
{
    if(stop())
    {
        QStringList args(_M_name);
        args << _M_args << "-auth" << _M_auth;

        _M_process.start(_M_path, args);
        wait_for_started(3000);
        wait_for_stopped(2000);

        for(int i=0; i<10; ++i)
        {
            if(!running()) break;

            _M_display= XOpenDisplay(_M_name.toAscii());
            if(_M_display) return true;

            sleep(1);
        }
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool Server::stop()
{
    if(running())
    {
        if(ready())
        {
            XCloseDisplay(_M_display);
            _M_display= nullptr;
        }

        signal(SIGHUP, SIG_IGN);
        killpg(getpid(), SIGHUP);

        killpg(_M_process.pid(), SIGTERM);
        bool stopped= wait_for_stopped(3000);

        if(!stopped)
        {
            killpg(_M_process.pid(), SIGKILL);
            stopped= wait_for_stopped(2000);
        }

        return stopped;
    }
    else return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}
