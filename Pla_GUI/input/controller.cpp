#include "controller.h"
#include "config.h"

#include "mainwindow.h"
#include "serial.h"
#include "traymessage.h"

#include <chrono>
#include <QKeyEvent>
#include <QThread>

using namespace std::chrono_literals;

int Controller::currentPG = 0;
std::atomic<SDL_Joystick *> Controller::joystick;
std::atomic_bool Controller::runThreads;
std::atomic_bool Controller::disableController;
std::thread Controller::connectionThread;
std::thread Controller::controllerThread;

JoystickTracker Controller::Left;
JoystickTracker Controller::Right;
PrimaryJoystickTracker Controller::Primary;
SteeringTracker Controller::Steering;
QColor Controller::Color;
int Controller::ColorBrightness;
bool Controller::ColorEnable;

bool Controller::init(void)
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_JOYSTICK) != 0)
        return false;

    joystick.store(nullptr);
    runThreads.store(true);
    disableController.store(false);
    connectionThread = std::thread(handleConnections);
    controllerThread = std::thread(handleController);

    // Allow time to find connected joystick
    std::this_thread::sleep_for(2s);
    return connected();
}

void Controller::end(void)
{
    runThreads.store(false);
    controllerThread.join();
    connectionThread.join();

    SDL_Quit();
}

bool Controller::connected(void)
{
    return joystick.load() != nullptr;
}

void Controller::selectPG(unsigned int pg)
{
    if (pg < 8) {
        currentPG = pg;
        Primary.setPG(pg);
    }
}

void Controller::save(QSettings& settings)
{
    settings.beginGroup("keys");

    // Save left aux
    settings.beginGroup("leftaux");
    Left.save(settings);
    settings.endGroup();

    // Save right aux
    settings.beginGroup("rightaux");
    Right.save(settings);
    settings.endGroup();

    // Save primary
    settings.beginGroup("primary");
    Primary.save(settings);
    settings.endGroup();

    // Save steering
    settings.beginGroup("steering");
    Steering.save(settings);
    settings.endGroup();

    settings.endGroup();
    settings.beginGroup("color");

    // Load colors
    settings.setValue("red", Color.red());
    settings.setValue("green", Color.green());
    settings.setValue("blue", Color.blue());
    settings.setValue("brightness", ColorBrightness);
    settings.setValue("enabled", ColorEnable);

    settings.endGroup();
}

void Controller::load(QSettings& settings)
{
    settings.beginGroup("keys");

    // Load left aux
    settings.beginGroup("leftaux");
    Left.load(settings);
    settings.endGroup();

    // Load right aux
    settings.beginGroup("rightaux");
    Right.load(settings);
    settings.endGroup();

    // Load primary
    settings.beginGroup("primary");
    Primary.load(settings);
    settings.endGroup();

    // Load steering
    settings.beginGroup("steering");
    Steering.load(settings);
    settings.endGroup();

    settings.endGroup();
    settings.beginGroup("color");

    // Load colors
    Color.setRed(settings.value("red", 0x03).toInt());
    Color.setGreen(settings.value("green", 0xF7).toInt());
    Color.setBlue(settings.value("blue", 0xFF).toInt());
    ColorBrightness = settings.value("brightness", 25).toInt();
    ColorEnable = settings.value("enabled", true).toBool();
    updateColor();

    settings.endGroup();
}

void Controller::updateColor(void)
{
    if (connected()) {
        if (ColorEnable) {
            auto r = static_cast<unsigned char>(Color.red() * ColorBrightness / 100);
            auto g = static_cast<unsigned char>(Color.green() * ColorBrightness / 100);
            auto b = static_cast<unsigned char>(Color.blue() * ColorBrightness / 100);
            Serial::sendColor(r, g, b);
        } else {
            Serial::sendColor(0, 0, 0);
        }
    }
}

void Controller::setOperating(bool enable)
{
    Left.setEnabled(enable);
    Right.setEnabled(enable);
    Primary.getPG().setEnabled(enable);
    Steering.setEnabled(enable);
}

void Controller::handleController(void)
{
    while (runThreads.load()) {
        // Only update if a joystick is connected
        auto* js = joystick.load();
        if (js == nullptr) {
            std::this_thread::sleep_for(config::ConnectionCheckFrequency);
        } else {
            SDL_JoystickUpdate();

            // Check for PG button presses
            for (int i = 3; i <= 10; i++) {
                if (SDL_JoystickGetButton(js, i)) {
                    if (currentPG != i - 3) {
                        currentPG = i - 3;
                        Primary.setPG(currentPG);
                    }
                    break;
                }
            }

            if (!disableController.load()) {
                // Update the joystick objects with their respective axes
                // Y-axis is inverted because joysticks on prototype are upside-down
                Left.update(-SDL_JoystickGetAxis(js, 3), SDL_JoystickGetAxis(js, 4),
                    SDL_JoystickGetButton(js, 2));
                Right.update(-SDL_JoystickGetAxis(js, 2), SDL_JoystickGetAxis(js, 5),
                    SDL_JoystickGetButton(js, 0));
                Primary.getPG().update(-SDL_JoystickGetAxis(js, 0), SDL_JoystickGetAxis(js, 1),
                    SDL_JoystickGetButton(js, 1));
                Steering.update(SDL_JoystickGetAxis(js, 6));
            }

            std::this_thread::sleep_for(config::InputUpdateFrequency);
        }
    }
}

void Controller::handleConnections(void)
{
    auto *tray = new TrayMessage();

    while (runThreads.load()) {
        for (SDL_Event event; SDL_PollEvent(&event);) {
            switch (event.type) {
            case SDL_JOYDEVICEADDED:
                if (joystick.load() == nullptr && checkGUID(event.jdevice.which)) {
                    if (Serial::open()) {
                        tray->show("PLA", "Controller connected!");
                        joystick.store(SDL_JoystickOpen(event.jdevice.which));
                        Serial::sendLights(true);
                        selectPG(Serial::getPg());
                        updateColor();
                    }
                }
                break;
            case SDL_JOYDEVICEREMOVED:
                if (joystick.load() != nullptr) {
                    tray->show("PLA", "Controller disconnected.");
                    Serial::sendLights(false);
                    Serial::close();
                    SDL_JoystickClose(joystick);
                    joystick.store(nullptr);
                }
                break;
            default:
                break;
            }
        }

        std::this_thread::sleep_for(config::ConnectionCheckFrequency);
    }

    delete tray;
    auto js = joystick.load();
    if (js != nullptr) {
        Serial::sendLights(false);
        Serial::close();
        SDL_JoystickClose(js);
        joystick.store(nullptr);
    }
}

bool Controller::checkGUID(int id)
{
        auto guid = SDL_JoystickGetDeviceGUID(id);
        return (guid.data[4] == config::DeviceGUID[0][0] &&
                guid.data[5] == config::DeviceGUID[0][1] &&
                guid.data[8] == config::DeviceGUID[0][2] &&
                guid.data[9] == config::DeviceGUID[0][3])
               ||
               (guid.data[4] == config::DeviceGUID[1][0] &&
                guid.data[5] == config::DeviceGUID[1][1] &&
                guid.data[8] == config::DeviceGUID[1][2] &&
                guid.data[9] == config::DeviceGUID[1][3]);
}
