/**
 * @file primaryjoysticktracker.h
 * @brief Provides additional support for the primary joystick.
 */
#ifndef PRIMARYJOYSTICKTRACKER_H
#define PRIMARYJOYSTICKTRACKER_H

#include "joysticktracker.h"

#include <array>
#include <atomic>

/**
 * @class PrimaryJoystickTracker
 * @brief Builds upon JoystickTracker to handle programmable groups (PGs).
 */
class PrimaryJoystickTracker {
public:
    PrimaryJoystickTracker();

    void setPG(int pg);
    JoystickTracker& getPG(int pg = -1);

    int getCurrentPG() const { return currentPG; }

    /**
     * Saves all actions to the given configuration file
     * @param settings The settings object to write to
     */
    void save(QSettings &settings) const;

    /**
     * Loads all actions from the given configuration file
     * @param settings The settings object to read from
     */
    void load(QSettings &settings);

    bool operator==(const PrimaryJoystickTracker& other);
    bool operator!=(const PrimaryJoystickTracker& other);

private:
    // Stores each PG's configuration.
    std::array<JoystickTracker, 8> groups;

    // Stores the current PG.
    std::atomic_int currentPG;
};

#endif // PRIMARYJOYSTICKTRACKER_H
