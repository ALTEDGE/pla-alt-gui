/**
 * @file serial.h
 * @brief Serial communication and management.
 */
#ifndef SERIAL_H
#define SERIAL_H

#ifdef PLA_WINDOWS
typedef struct IUnknown IUnknown;
#include <Windows.h>
#endif

#include <string>

/**
 * @class Serial
 * @brief Provides functions to communicate with the controller over a serial
 * connection.
 */
class Serial {
public:
    /**
     * Attempts to open a serial connection with the controller.
     * @return true if success
     */
    static bool open(void);

    /**
     * Closes the serial connection to the controller.
     */
    static void close(void);

    /**
     * Tests if the serial connection exists.
     */
    static bool connected(void);

    /**
     * Sends the controller a command to update the RGB LEDs to the given values.
     * @param r Red value, 0-255
     * @param g Green value, 0-255
     * @param b Blue value, 0-255
     */
    static void sendColor(unsigned char r, unsigned char g, unsigned char b);

    /**
     * Sends the most recently sent color.
     */
    static void sendColor(void);

    /**
     * Sends code for showing 'connected/disconnected' lights on the
     * controller.
     * @param on True if connected (show user lights), false if not (constant
     *           green)
     */
    static void sendLights(bool on);

    static int getPg();

private:
#ifdef PLA_WINDOWS
    static HANDLE hComPort;
#else
    static int comFd;
#endif // PLA_WINDOWS
    static unsigned char colorBuffer[4];

    static std::string nativeOpen(void);

    static void nativeWrite(unsigned char *array, unsigned int count);
    static void nativeRead(unsigned char *array, unsigned int count);

};

#endif // SERIAL_H
