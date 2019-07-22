/***
 * This file contains the following classes:
 *
 * Joystick, JoystickTracker, PrimaryJoystickTracker, and SteeringTracker.
 **/

#ifndef JOYSTICKTRACKER_H
#define JOYSTICKTRACKER_H

#include "joystick.h"
#include "keysender.h"

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
class JoystickTracker : protected Joystick, public KeySender<16>
{
public:
    JoystickTracker(bool sequencer = false, bool diagonal = false);
    virtual ~JoystickTracker(void) = default;

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

#endif // JOYSTICKTRACKER_H
