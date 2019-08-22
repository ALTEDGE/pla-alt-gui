/**
 * @file joystick.h
 * @brief Provides common joystick functionalities.
 */
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
    int shortThreshold;

    /**
     * Defines how far the joystick has to move from the origin on one of its
     * axes to trigger its "Vector 2" action.
     *
     * This threshold isn't used if vector sequencing is disabled.
     */
    int farThreshold;

public:
    /**
     * Gets the short threshold.
     */
    int getShortThreshold(void) const;

    /**
     * Gets the far threshold.
     */
    int getFarThreshold(void) const;

    /**
     * Sets the short threshold to the given value.
     * saveThresholds() must be called to retain the new value.
     */
    void setShortThreshold(int value);
    /**
     * Sets the far threshold to the given value.
     * saveThresholds() must be called to retain the new value.
     */
    void setFarThreshold(int value);

protected:
    /**
     * Loads threshold values from the given settings object.
     * @param config Settings to load from
     */
    void loadThresholds(QSettings& config);
    /**
     * Saves threshold values to the given settings object.
     * @param config Settings to save to
     */
    void saveThresholds(QSettings& config) const;
};

#endif // JOYSTICK_H
