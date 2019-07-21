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
    using namespace std::chrono_literals;

    /**
     * Controls delay between controller input polling.
     */
    constexpr auto InputUpdateFrequency = 10ms;
    /**
     * Delay between checking for a connected joystick.
     */
    constexpr auto ConnectionCheckFrequency = 1s;

    /**
     * Minimum delay between macro key presses/releases.
     */
    constexpr auto MinimumMacroDelay = 5u/*ms*/;

    /**
     * USB vendor and device ID for checking proper joystick connection.
     */
    constexpr unsigned char DeviceGUID[4] = { 0x4F, 0x1B, 0x04, 0x92 };
    /**
     * USB IDs for Windows COM checking.
     */
    constexpr const char WindowsDeviceGUID[18] = "VID_1B4F&PID_9204";

    /**
     * Defines the minimum rate-of-change in a joystick to be considered movement.
     * Actions will not be fired unless the joystick is moving at a lesser speed.
     *
     * Note: Axes in SDL have a range of -32767 to +32767
     */
    constexpr double JoystickSpeedThreshold = 100;

    constexpr int JoystickDefaultShortThreshold = 10000;
    constexpr int JoystickDefaultFarThreshold = 30000;
}

#endif // CONFIG_H
