#include "controllerroutine.h"

#include "mainwindow.h"
#include "serial.h"

#include <chrono>
#include <QKeyEvent>

using namespace std::chrono_literals;

unsigned int Controller::currentPG = 0;
std::atomic<SDL_Joystick *> Controller::joystick;
std::atomic_bool Controller::runThreads;
std::thread Controller::connectionThread;
std::thread Controller::controllerThread;

JoystickTracker Controller::Left;
JoystickTracker Controller::Right;
PrimaryJoystickTracker Controller::Primary;
SteeringTracker Controller::Steering;

bool Controller::init(void)
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_JOYSTICK) != 0)
        return false;

    joystick.store(nullptr);
    runThreads.store(true);
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

    // Save threshold values
    Joystick::saveThresholds(settings);
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

    // Load threshold values
    Joystick::loadThresholds(settings);
}

void Controller::handleController(void)
{
    while (runThreads.load()) {
        // Only update if a joystick is connected
        if (joystick == nullptr) {
            std::this_thread::sleep_for(500ms);
        } else {
            SDL_JoystickUpdate();

            // Check for PG button presses
            for (unsigned int i = 3; i <= 10; i++) {
                if (SDL_JoystickGetButton(joystick, static_cast<int>(i))) {
                    if (currentPG != i - 3) {
                        currentPG = i - 3;
                        Primary.setPG(currentPG);
                    }
                    break;
                }
            }

            // Update the joystick objects with their respective axes
            // Y-axis is inverted because joysticks on prototype are upside-down
            Left.update(SDL_JoystickGetAxis(joystick, 3), -SDL_JoystickGetAxis(joystick, 4));
            Right.update(SDL_JoystickGetAxis(joystick, 2), -SDL_JoystickGetAxis(joystick, 5));
            Primary.update(SDL_JoystickGetAxis(joystick, 0), -SDL_JoystickGetAxis(joystick, 1));
            Steering.update(SDL_JoystickGetAxis(joystick, 6));

            std::this_thread::sleep_for(10ms);
        }
    }
}

void Controller::handleConnections(void)
{
    while (runThreads.load()) {
        for (SDL_Event event; SDL_PollEvent(&event);) {
            switch (event.type) {
            case SDL_JOYDEVICEADDED:
                std::cout << "Joystick added: " << event.jdevice.which << std::endl;
                if (joystick.load() == nullptr && checkGUID(event.jdevice.which)) {
                    Serial::open();
                    joystick.store(SDL_JoystickOpen(event.jdevice.which));
                }
                break;
            case SDL_JOYDEVICEREMOVED:
                std::cout << "Joystick removed: " << event.jdevice.which << std::endl;
                if (joystick.load() != nullptr) {
                    SDL_JoystickClose(joystick);
                    joystick.store(nullptr);
                }
                break;
            default:
                break;
            }
        }

        std::this_thread::sleep_for(1s);
    }

    if (auto js = joystick.load(); js != nullptr) {
        SDL_JoystickClose(js);
        Serial::close();
    }
}

bool Controller::checkGUID(int id)
{
        auto guid = SDL_JoystickGetDeviceGUID(id);
        return (guid.data[4] == deviceGUID[0] &&
            guid.data[5] == deviceGUID[1] &&
            guid.data[8] == deviceGUID[2] &&
            guid.data[9] == deviceGUID[3]);
}
