/**
 * @file primaryjoysticktracker.h
 * @brief Provides additional support for the primary joystick.
 */
#ifndef PRIMARYJOYSTICKTRACKER_H
#define PRIMARYJOYSTICKTRACKER_H

#include "joysticktracker.h"

/**
 * @class PrimaryJoystickTracker
 * @brief Builds upon JoystickTracker to handle programmable groups (PGs).
 */
class PrimaryJoystickTracker : public JoystickTracker {
public:
    PrimaryJoystickTracker(bool sequencer = false, bool diagonal = false);

    /**
     * Sets the index'th action under the given PG to the given key.
     * @param pg The PG to modify, 0-7
     * @param index The index to modify, 0-15
     * @param key The key to set the action to (Key() for no action)
     */
    void setPGKey(int pg, int index, Key key);

    /**
     * Sets the current PG.
     * @param pg The new PG, 0-7
     */
    void setPG(int pg);

    /**
     * Returns a string describing the index'th action under the given PG.
     * @param pg The PG to access
     * @param index The action's index
     * @return A string to describe the action
     */
    QString getText(int pg, int index) const;

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

    // "Equal" comparison overload, needed for Editing objects
    bool operator==(const PrimaryJoystickTracker& other) {
        return groups == other.groups &&
            getDiagonals() == other.getDiagonals() &&
            getSequencing() == other.getSequencing();
    }

    // "Not Equal" comparison overload, needed for Editing objects
    bool operator!=(const PrimaryJoystickTracker& other) {
        return groups != other.groups ||
            getDiagonals() != other.getDiagonals() ||
            getSequencing() != other.getSequencing();
    }

private:
    // Stores actions for all eight PGs.
    std::array<std::array<Key, 16>, 8> groups;

    // Stores the current PG.
    int currentPG;
};

#endif // PRIMARYJOYSTICKTRACKER_H
