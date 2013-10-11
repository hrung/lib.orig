///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "application.h"

#include "message.h"
#include "except.h"

#include <QMessageBox>

///////////////////////////////////////////////////////////////////////////////////////////////////
Application::Application(int& argc, char* argv[]):
    QApplication(argc, argv)
{ }

///////////////////////////////////////////////////////////////////////////////////////////////////
int Application::run()
{
    try
    {
        int code= proc();
        if(_M_e) std::rethrow_exception(_M_e);

        return code;
    }
    catch(except::user_exception& e)
    {
        show_error(e);
        return message(e.user_message());
    }
    catch(except::exception& e)
    {
        show_error(e);
        if(e.message().size())
            return message(e.message());
        else return message(e.what());
    }
    catch(std::exception& e)
    {
        show_error(e.what());
        return message(e.what());
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int Application::message(const std::string& message)
{
    QMessageBox::critical(0, "Error", QString::fromStdString(message));
    return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool Application::notify(QObject* receiver, QEvent* event)
{
    // Qt does not allow exceptions thrown from event handlers to be processed outside event loop
    // So we catch them here, save them and tell the application to exit
    try
    {
        return QApplication::notify(receiver, event);
    }
    catch(...)
    {
        _M_e= std::current_exception();
        exit();
    }
    return false;
}
