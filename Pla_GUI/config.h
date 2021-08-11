/**
 * @file config.h
 * @brief Provides global configuration values.
 */
#ifndef CONFIG_H
#define CONFIG_H

#include <chrono>

/**
 * Uncomment to let the program run in the background, minimized to the system
 * tray area on the start bar.
 */
#define KEEP_OPEN_IN_TRAY

namespace config
{
    /**
     * Version string to show in bottom right of the main window.
     */
    constexpr const char *versionString = "v0.5.2";



    using namespace std::chrono_literals;

    /**
     * Controls delay between controller input polling.
     */
    constexpr auto InputUpdateFrequency = 10ms;
    /**
     * Delay after sending each key event.
     * This may not be necessary; it's just to be safe.
     */
    constexpr auto InputSendDelay = 1ms;
    /**
     * Delay between checking for a connected joystick.
     */
    constexpr auto ConnectionCheckFrequency = 1s;

    /**
     * Minimum delay between macro key presses/releases.
     */
    constexpr auto MinimumMacroDelay = 2ms;

    /**
     * USB vendor and device ID for checking proper joystick connection.
     */
    constexpr unsigned char DeviceGUID[4] = //{ 0x09, 0x12, 0x01, 0x00 };
                                            { 0x4F, 0x1B, 0x04, 0x92 };
    /**
     * USB IDs for Windows COM checking.
     */
    constexpr const char WindowsDeviceGUID[18] = //"VID_1209&PID_0001";
                                                 "VID_1B4F&PID_9204";

    /**
     * Defines the minimum rate-of-change in a joystick to be considered movement.
     * Actions will not be fired unless the joystick is moving at a lesser speed.
     *
     * Note: Axes in SDL have a range of -32767 to +32767
     */
    constexpr double JoystickSpeedThreshold = 100;

    /**
     * Default threshold value for vector 1 / normal actions.
     */
    constexpr int JoystickDefaultShortThreshold = static_cast<int>(32767 * 0.6f);

    /**
     * Default threshold value for vector 2.
     */
    constexpr int JoystickDefaultFarThreshold = static_cast<int>(32767 * 0.9f);

    /**
     * Time to show tray messages, in milliseconds.
     */
    //constexpr unsigned int TrayMessageDuration = 1000;
}

#endif // CONFIG_H
