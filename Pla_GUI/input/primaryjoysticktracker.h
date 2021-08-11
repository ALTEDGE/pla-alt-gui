/**
 * @file primaryjoysticktracker.h
 * @brief Provides additional support for the primary joystick.
 */
#ifndef PRIMARYJOYSTICKTRACKER_H
#define PRIMARYJOYSTICKTRACKER_H

#include "joysticktracker.h"

#include <array>

/**
 * @class PrimaryJoystickTracker
 * @brief Builds upon JoystickTracker to handle programmable groups (PGs).
 */
class PrimaryJoystickTracker : public JoystickTracker {
public:
    PrimaryJoystickTracker(bool sequencer = false, bool diagonal = false);

    /**
     * Sets the current PG.
     * @param pg The new PG, 0-7
     */
    void setPG(int pg);

    void saveCurrentPG();

    /**
     * Saves all actions to the given configuration file
     * @param settings The settings object to write to
     */
    void save(QSettings &settings) const final;

    /**
     * Loads all actions from the given configuration file
     * @param settings The settings object to read from
     */
    void load(QSettings &settings) final;

    bool operator==(const PrimaryJoystickTracker& other);
    bool operator!=(const PrimaryJoystickTracker& other);

private:
    // Stores each PG's configuration.
    // Stores actions for all eight PGs.
    std::array<JoystickTracker, 8> groups;

    // Stores the current PG.
    int currentPG;
};

#endif // PRIMARYJOYSTICKTRACKER_H
