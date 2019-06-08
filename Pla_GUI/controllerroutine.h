#ifndef CONTROLLERROUTINE_H
#define CONTROLLERROUTINE_H

#include <QKeyEvent>
#include <QObject>
#include <QSettings>

#include <SDL2/SDL.h>
#include <iostream>

#include "joysticktracker.h"

/**
 * @class Controller
 * @brief Handles all non-serial communication with the controller.
 */
class Controller {
private:
    /**
     * Defines the USB vendor/product ID of the controller. Initialization will
     * fail if SDL does not find a controller matching this ID.
     */
    constexpr static unsigned char deviceGUID[4] = {
        0x4F, 0x1B, 0x04, 0x92
    };

public:
    // Objects to manage controller joysticks & wheel
    static JoystickTracker Left;
    static JoystickTracker Right;
    static PrimaryJoystickTracker Primary;
    static SteeringTracker Steering;

    /**
     * Initializes SDL and searches for a connected controller.
     * @return True if success
     */
    static bool init(void);

    /**
     * Closes joystick communication and shuts down SDL.
     */
    static void end(void);

    /**
     * Tests if ControllerRoutine has been successfully initialized.
     * @return True if connected
     */
    static bool good(void);

    /**
     * Saves all settings to the given settings handler.
     * @param settings Where to save settings to
     */
    static void save(QSettings& settings);

    /**
     * Loads all actions from the given settings handler.
     * @param settings WHere to load settings from
     */
    static void load(QSettings& settings);

    /**
     * Updates controller input and fires joystick or button actions as necessary.
     * Should be called in a loop to continuously update.
     */
    static void update(void);

private:
    /**
     * Keeps track of the currently selected PG.
     */
    static unsigned int currentPG;

    static SDL_Joystick *joystick;
};

#endif // CONTROLLERROUTINE_H
