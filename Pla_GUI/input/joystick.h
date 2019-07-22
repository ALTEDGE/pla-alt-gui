#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <QSettings>

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

#endif // JOYSTICK_H
