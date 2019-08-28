#include "mainwindow.h"
#include "controller.h"
#include "macro.h"
#include "profile.h"
#include "runguard.h"
#include "serial.h"

#include <QApplication>
#include <QFile>
#include <QMessageBox>

#include <SDL2/SDL.h>
#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

// Windows complains when compiling because both Qt and SDL try to define
// their own main functions, so we override them by undefining main here.
#ifdef _WIN64
#undef main
#endif

int main(int argc, char *argv[])
{
    // Base initialization, and stylesheet loading
    QApplication a (argc, argv);
    QFile styleSheet ("stylesheet");
    styleSheet.open(QFile::ReadOnly);
    a.setStyleSheet(QString(styleSheet.readAll()));

    // Check if an instance is already running
    RunGuard program ("PLA GUI");
    if (program.alreadyRunning()) {
        QMessageBox::information(nullptr, "PLA GUI",
            "PLA GUI is already running.\n"
            "The program may be accessed through the system tray.",
            QMessageBox::Ok);
        return 0;
    }

    // Load controller settings
    Profile::openFirst();
    Macro::load(Profile::current());
    Controller::load(Profile::current());

    // Attempt to connect to the controller
    if (!Controller::init()) {
        // No controller
        QMessageBox::information(nullptr, "Controller Disconnected",
             "Unable to find the PLA controller. Please connect the controller "
             "to use it with this program.", QMessageBox::Ok);
    }

    // Show the main window
    MainWindow w;
    w.show();
    auto ret = a.exec();

    // Close connections when finished
    Controller::end();

    return ret;
}
