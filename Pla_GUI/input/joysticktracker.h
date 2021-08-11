/**
 * @file joysticktracker.h
 * @brief Provides class to track joystick position and translate from position
 * to action index.
 */
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
class JoystickTracker : public Joystick, public KeySender
{
public:
    /**
     * Constructs a tracker with the given initial sequencer and diagonal
     * states.
     * @param sequencer True to have the vector sequencer enabled
     * @param diagonal True to have diagonal movement enabled
     */
    JoystickTracker(bool sequencer = false, bool diagonal = false);
    virtual ~JoystickTracker(void) = default;

    /**
     * Enables or disables vector sequencing.
     * @param enable True to enable sequencing
     */
    void setSequencing(bool enable);

    /**
     * Enables or disables diagonals.
     * If enabled, actions cannot be assigned to diagonal directions. Instead,
     * diagonal positions will trigger the two adjacent actions.
     * @param enable True to enable sequencing
     */
    void setDiagonals(bool enable);

    inline void setButtonSticky(bool yes) {
        isButtonSticky = yes;
    }

    inline void setEnabled(bool yes) {
        isEnabled = yes;
    }

    /**
     * Checks if the vector sequencer is enabled.
     * @return True if sequencing is enabled
     */
    inline bool getSequencing(void) const
    { return useSequencing; }

    /**
     * Checks if diagonal movement is enabled.
     * @return True if diagonals are enabled
     */
    inline bool getDiagonals(void) const
    { return useDiagonals; }

    inline bool getButtonSticky(void) const
    { return isButtonSticky; }

    /**
     * Updates the tracker with the given values, and fires an action if
     * necessary.
     * @param x The x-axis' new position
     * @param y The y-axis' new position
     * @param pressed State of the joystick's button
     */
    void update(int x, int y, int pressed);

    /**
     * Saves settings to the given settings object.
     */
    virtual void save(QSettings &settings) const;

    /**
     * Loads settings from the given settings object.
     */
    virtual void load(QSettings &settings);

    // "Equal" comparison overload, needed for Editing objects
    bool operator==(const JoystickTracker& other) const {
        return KeySender::operator==(other) &&
            useSequencing == other.useSequencing &&
            useDiagonals == other.useDiagonals &&
            isButtonSticky == other.isButtonSticky;
    }

    // "Not Equal" comparison overload, needed for Editing objects
    bool operator!=(const JoystickTracker& other) const {
        return KeySender::operator!=(other) ||
            useSequencing != other.useSequencing ||
            useDiagonals != other.useDiagonals ||
            isButtonSticky != other.isButtonSticky;
    }

    JoystickTracker& operator=(const JoystickTracker& other) {
        *dynamic_cast<KeySender *>(this) = other;
        *dynamic_cast<Joystick *>(this) = other;
        useSequencing = other.useSequencing;
        useDiagonals = other.useDiagonals;
        isButtonSticky = other.isButtonSticky;
        return *this;
    }

    /**
     * Gets the last position of the joystick.
     * @return A pair of the x/y position
     */
    inline std::pair<int, int> getPosition(void) const {
        return { lastX, lastY };
    }

    /**
     * Dumps the joystick state to standard output.
     * @param id Character id to include in the dump (useful for identification)
     */
    //void dumpState(char id) const;

private:
    // The previous X and Y position, for tracking velocity.
    int lastX = 0;
    int lastY = 0;
    int lastPressed = 0;

    // If true, vector sequencing is enabled.
    bool useSequencing = false;

    // If true, diagonal actions are disabled.
    // Instead, a diagonal movement triggers the two adjacent actions (e.g. up
    // and left).
    bool useDiagonals = false;

    bool isButtonSticky = false;
    bool stickyState = false;
    bool isEnabled = true;

    /**
     * Gets the index of an action to trigger, based on the axes' states.
     * @param hstate The horizontal axis' toState() value
     * @param vstate The vertical axis' toState() value
     * @return If -1, no action to trigger, otherwise, an action's index
     */
    int getActionIndex(int hstate, int vstate) const;

    /**
     * Returns a value with bits set based on what directions are active.
     * Used when useDiagonals == true.
     *
     * @param hstate The horizontal axis' toState() value
     * @param vstate The vertical axis' toState() value
     * @return A 16-bit value where bit 'n' is set if action 'n' should
     * be fired.
     */
    int getActionBits(int hstate, int vstate) const;
};

#endif // JOYSTICKTRACKER_H
