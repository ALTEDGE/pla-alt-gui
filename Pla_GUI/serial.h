#ifndef SERIAL_H
#define SERIAL_H

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

    static bool connected(void);

    /**
     * Sends the controller a command to update the RGB LEDs to the given values.
     * @param r Red value, 0-255
     * @param g Green value, 0-255
     * @param b Blue value, 0-255
     */
    static void sendColor(unsigned char r, unsigned char g, unsigned char b);

private:
#ifdef _WIN64
    static HANDLE hComPort;
#else
    static int comFd;
#endif // _WIN64

    static std::string nativeOpen(void);

    static void nativeWrite(unsigned char *array, unsigned int count);

};

#endif // SERIAL_H
