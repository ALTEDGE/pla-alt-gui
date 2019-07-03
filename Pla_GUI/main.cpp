#include "mainwindow.h"
#include "controllerroutine.h"
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
    std::thread updateThread;

    // Attempt to connect to the controller
    if (!Controller::init()) {
        // No controller
        QMessageBox::information(nullptr, "Controller Disconnected", "Unable to find the PLA \
controller. The program will not work with the controller unless it is connected at \
start.", QMessageBox::Ok);
    }

    // Load GUI stylesheet
    QFile styleSheet ("stylesheet");
    styleSheet.open(QFile::ReadOnly);
    a.setStyleSheet(QLatin1String(styleSheet.readAll()));

    // Show the main window
    MainWindow w;
    w.show();
    auto ret = a.exec();

    // Close connections when finished
    Controller::end();

    return ret;
}
