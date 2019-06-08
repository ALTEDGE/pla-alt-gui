#include "controllerroutine.h"

#include "mainwindow.h"

#include <QKeyEvent>

unsigned int Controller::currentPG = 0;
SDL_Joystick *Controller::joystick = nullptr;

JoystickTracker Controller::Left;
JoystickTracker Controller::Right;
PrimaryJoystickTracker Controller::Primary;
SteeringTracker Controller::Steering;

bool Controller::init(void)
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_JOYSTICK) != 0)
        return false;

    // Search for a matching GUID
    int id;
    int devCount = SDL_NumJoysticks();
    for (id = 0; id < devCount; id++) {
        auto guid = SDL_JoystickGetDeviceGUID(id);
        if (guid.data[4] == deviceGUID[0] &&
            guid.data[5] == deviceGUID[1] &&
            guid.data[8] == deviceGUID[2] &&
            guid.data[9] == deviceGUID[3])
            break;
    }

    if (id == devCount)
        return false;

    // Open a connection to the controller
    joystick = SDL_JoystickOpen(id);

    return joystick != nullptr;
}

void Controller::end(void)
{
    SDL_JoystickClose(joystick);
    joystick = nullptr;

    SDL_Quit();
}

bool Controller::good(void)
{
    return joystick != nullptr;
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

void Controller::update(void)
{
    // Only update if properly initialized
    if (joystick == nullptr)
        return;

    SDL_JoystickUpdate();

    // Check for PG button presses
    for (unsigned int i = 3; i <= 10; i++) {
        if (SDL_JoystickGetButton(joystick, i) && currentPG != i - 3) {
            currentPG = i - 3;
            Primary.setPG(currentPG);
            break;
        }
    }

    // Update the joystick objects with their respective axes
    // Y-axis is inverted because joysticks on prototype are upside-down
    Left.update(SDL_JoystickGetAxis(joystick, 3), -SDL_JoystickGetAxis(joystick, 4));
    Right.update(SDL_JoystickGetAxis(joystick, 2), -SDL_JoystickGetAxis(joystick, 5));
    Primary.update(SDL_JoystickGetAxis(joystick, 0), -SDL_JoystickGetAxis(joystick, 1));
    Steering.update(SDL_JoystickGetAxis(joystick, 6));
}
