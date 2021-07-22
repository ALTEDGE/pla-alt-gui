/**
 * @file steeringtracker.h
 * @brief Provides tracking functionality specific to the steering wheel.
 */
#ifndef STEERINGTRACKER_H
#define STEERINGTRACKER_H

#include "joystick.h"
#include "keysender.h"

/**
 * @class SteeringTracker
 * @brief Tracks steering wheel movement, firing actions when necesasary.
 */
class SteeringTracker : public Joystick, public KeySender
{
public:
    /**
     * Creates a tracker with the given digital/analog state.
     * @param d True for digital mode, false for analog
     */
    SteeringTracker(bool d = true);
    virtual ~SteeringTracker(void) = default;

    /**
     * Enables/disables digital steering.
     * @param v True for digital, false for analog
     */
    void setDigital(bool v);

    /**
     * Gets the state of digital steering.
     */
    bool getDigital(void) const;

    inline void setEnabled(bool yes) {
        isEnabled = yes;
    }

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

    /**
     * Gets the last recorded position of the steering wheel.
     */
    inline int getPosition(void) const {
        return lastPosition;
    }

private:
    // When true, digital steering is enabled.
    bool digital;
    bool isEnabled = true;

    int lastPosition;
};


#endif // STEERINGTRACKER_H
