/***
 * This file contains the following classes:
 *
 * Joystick, JoystickTracker, PrimaryJoystickTracker, and SteeringTracker.
 **/

#ifndef JOYSTICKTRACKER_H
#define JOYSTICKTRACKER_H

#include "key.h"

/**
 * @class Joystick
 * @brief Provides common data between the joystick-managing classes.
 */
class Joystick {
protected:
    /**
     * Defines how far the joystick has to move from the origin on one of its
     * axes to trigger an action.
     *
     * If vector sequencing is disabled, passing this threshold will trigger
     * that direction's action.
     * Otherwise, passing will trigger the "Vector 1" action.
     */
    static int shortThreshold;

    /**
     * Defines how far the joystick has to move from the origin on one of its
     * axes to trigger its "Vector 2" action.
     *
     * This threshold isn't used if vector sequencing is disabled.
     */
    static int farThreshold;

public:
    /**
     * Gets the short threshold.
     */
    static int getShortThreshold(void);
    /**
     * Gets the far threshold.
     */
    static int getFarThreshold(void);

    /**
     * Sets the short threshold to the given value.
     * saveThresholds() must be called to retain the new value.
     */
    static void setShortThreshold(int value);
    /**
     * Sets the far threshold to the given value.
     * saveThresholds() must be called to retain the new value.
     */
    static void setFarThreshold(int value);

    /**
     * Loads threshold values from the given settings object.
     * @param config Settings to load from
     */
    static void loadThresholds(QSettings& config);
    /**
     * Saves threshold values to the given settings object.
     * @param config Settings to save to
     */
    static void saveThresholds(QSettings& config);
};



/**
 * @class JoystickTracker
 * @brief Tracks movement of a two-axis joystick and fires assignable keystrokes.
 *
 * A joystick can fire either 16 actions (via vector sequencing), or 8 otherwise.
 *
 * Action indexes without sequencing:
 *      7   0   1
 *      6       2
 *      5   4   3
 *
 * for vector sequencing:
 *     15      8       9
 *         7   0   1
 *     14  6       2  10
 *         5   4   3
 *     13     12      11
 */
class JoystickTracker : protected Joystick, public KeySender<16> {
public:
    JoystickTracker(bool sequencer = false, bool diagonal = false);

    /**
     * Enables or disables vector sequencing.
     * @param enable True to enable sequencing
     */
    void setSequencing(bool enable);

    /**
     * Enables or disables diagonals.
     *
     * If enabled, actions cannot be assigned to diagonal directions. Instead,
     * diagonal positions will trigger the two adjacent actions.
     */
    void setDiagonals(bool enable);

    // Get the vector sequencing state
    inline bool getSequencing(void) const
    { return useSequencing; }

    // Get the diagonal state
    inline bool getDiagonals(void) const
    { return useDiagonals; }

    /**
     * Updates with the given values, and fires an action if necessary.
     * @param x The x-axis' new position
     * @param y The y-axis' new position
     */
    virtual void update(int x, int y);

    /**
     * Saves settings to the given settings object.
     */
    virtual void save(QSettings &settings) const;

    /**
     * Loads settings from the given settings object.
     */
    virtual void load(QSettings &settings);

    // "Equal" comparison overload, needed for Editing objects
    bool operator==(const JoystickTracker& other) {
        return KeySender::operator==(other) &&
            useDiagonals == other.useDiagonals &&
            useSequencing == other.useSequencing;
    }

    // "Not Equal" comparison overload, needed for Editing objects
    bool operator!=(const JoystickTracker& other) {
        return KeySender::operator!=(other) ||
            useDiagonals != other.useDiagonals ||
            useSequencing != other.useSequencing;
    }

protected:
    // Contains the previous X position, to track velocity.
    // Kept protected so ThresholdSetter can access it.
    int lastX;

private:
    /**
     * Contains the previous Y position, to track velocity.
     */
    int lastY;

    /**
     * If true, vector sequencing is enabled.
     */
    bool useSequencing;

    /**
     * If true, diagonal actions are disabled.
     * Instead, for example, an up-left movement would press the up and left
     * actions/keys.
     */
    bool useDiagonals;

    /**
     * Converts a raw axis value to a 'state'.
     * @param v The value to convert
     * @return The value's 'state'
     *
     * If state zero, the axis hasn't moved enough to trigger an action.
     * State +/- 1: Axis has passed the lowest action threshold.
     * State +/- 2: Axis has passed the higher threshold (used for two-
     *     stage mode)
     */
    static int toState(int v);

    /**
     * Gets the index of an action to trigger, based on the axes' states.
     * @param hstate The horizontal axis' toState() value
     * @param vstate The vertical axis' toState() value
     * @return If -1, no action to trigger, otherwise, an action's index
     */
    int getActionIndex(int hstate, int vstate);

    /**
     * Returns a value with bits set based on what directions are active.
     * Used when useDiagonals == true.
     *
     * @param hstate The horizontal axis' toState() value
     * @param vstate The vertical axis' toState() value
     * @return A 16-bit value where bit 'n' is set if action 'n' should
     * be fired.
     */
    int getActionBits(int hstate, int vstate);
};



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
    void setPGKey(unsigned int pg, unsigned int index, Key key);

    /**
     * Sets the current PG.
     * @param pg The new PG, 0-7
     */
    void setPG(unsigned int pg);

    /**
     * Returns a string describing the index'th action under the given PG.
     * @param pg The PG to access
     * @param index The action's index
     * @return A string to describe the action
     */
    QString getText(unsigned int pg, unsigned int index) const;

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

    /**
     * Gets the last position of the X-axis.
     * Used for ThresholdSetter.
     */
    int getLastX(void) const
    { return lastX; }

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
    /**
     * Stores actions for all eight PGs.
     */
    std::array<std::array<Key, 16>, 8> groups;

    /**
     * Stores the current PG.
     */
    unsigned int currentPG;
};



/**
 * @class SteeringTracker
 * @brief Tracks steering wheel movement, firing actions when necesasary.
 */
class SteeringTracker : public Joystick, public KeySender<2>
{
public:
    SteeringTracker(bool d = true);

    /**
     * Enables/disables digital steering.
     * @param v True for digital, false for analog
     */
    void setDigital(bool v);

    /**
     * Gets the state of digital steering.
     */
    bool getDigital(void) const;

    /**
     * Fires actions based on the wheel's position.
     * Analog steering requires no updates, so in analog mode this function will
     * do nothing.
     * @param pos The wheel's position
     */
    void update(int pos);

    /**
     * Saves settings to the given configuration file.
     * @param settings The file to save to
     */
    void save(QSettings &settings) const final;

    /**
     * Loads settings from the given configuration file.
     * @param settings The file to load from
     */
    void load(QSettings &settings) final;

    // "Equal" comparison overload, needed for Editing objects
    bool operator==(const SteeringTracker& other) {
        return KeySender::operator==(other) && digital == other.digital;
    }

    // "Not Equal" comparison overload, needed for Editing objects
    bool operator!=(const SteeringTracker& other) {
        return KeySender::operator!=(other) || digital != other.digital;
    }

private:
    /**
     * When true, digital steering is enabled.
     */
    bool digital;
};

#endif // JOYSTICKTRACKER_H
