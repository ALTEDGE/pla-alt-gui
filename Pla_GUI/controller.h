#ifndef CONTROLLERROUTINE_H
#define CONTROLLERROUTINE_H

#include <QKeyEvent>
#include <QObject>
#include <QSettings>

#include <SDL2/SDL.h>
#include <atomic>
#include <iostream>
#include <thread>

#include "joysticktracker.h"

/**
 * @class Controller
 * @brief Handles all non-serial communication with the controller.
 */
class Controller {
public:
    // Objects to manage controller joysticks & wheel
    static JoystickTracker Left;
    static JoystickTracker Right;
    static PrimaryJoystickTracker Primary;
    static SteeringTracker Steering;
    static QColor Color;
    static int ColorBrightness;
    static bool ColorEnable;

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
    static bool connected(void);

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

    static void updateColor(void);

private:
    /**
     * Keeps track of the currently selected PG.
     */
    static unsigned int currentPG;

    static std::atomic<SDL_Joystick *> joystick;
    static std::atomic_bool runThreads;
    static std::thread connectionThread;
    static std::thread controllerThread;

    static void handleConnections(void);
    static void handleController(void);

    static bool checkGUID(int id);
};

#endif // CONTROLLERROUTINE_H