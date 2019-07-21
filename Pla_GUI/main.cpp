#include "mainwindow.h"
#include "controller.h"
#include "macro.h"
#include "profile.h"
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
    QApplication a (argc, argv);

    // Load GUI stylesheet
    QFile styleSheet ("stylesheet");
    styleSheet.open(QFile::ReadOnly);
    a.setStyleSheet(QString(styleSheet.readAll()));

    // Load controller settings
    Profile::openFirst();
    Macro::load(Profile::current());
    Controller::load(Profile::current());

    // Show the main window
    // Construct before controller init so tray icon exists
    MainWindow w;

    // Attempt to connect to the controller
    if (!Controller::init()) {
        // No controller
        QMessageBox::information(nullptr, "Controller Disconnected", "Unable to find the PLA \
controller. The program will not work with the controller unless it is connected at \
start.", QMessageBox::Ok);
    }

    w.show();
    auto ret = a.exec();

    // Close connections when finished
    Controller::end();

    return ret;
}
