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
    std::atomic<bool> shouldRun;

    // Attempt to connect to the controller
    bool connected = Controller::init();

    // Only start controller routines if the controller is connected
    if (connected) {
        // Open a serial connection (for color modification)
        serial::open();

        // Start listening for controller actions
        shouldRun.store(true);
        updateThread = std::thread([&shouldRun](void) {
            while (shouldRun.load()) {
                Controller::update();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });
    } else {
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
    if (connected) {
        shouldRun.store(false);
        updateThread.join();

        serial::close();
        Controller::end();
    }

    return ret;
}
