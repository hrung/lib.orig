///////////////////////////////////////////////////////////////////////////////////////////////////
#include "application.h"

#include "common/common.h"
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
        show_message(e);
        return message(e.message(), e.user_message());
    }
    catch(except::exception& e)
    {
        show_message(e);
        return message(e.message());
    }
    catch(std::exception& e)
    {
        show_message(e.what(), level::error);
        return message(e.what());
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int Application::message(const std::string& message, const std::string& user_message)
{
    QMessageBox::critical(0, "Error", QString::fromStdString(user_message.empty()? message: user_message));
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
