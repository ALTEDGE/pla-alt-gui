#ifndef SERIAL_H
#define SERIAL_H

/**
 * @namespace serial
 * @brief Provides functions to communicate with the controller over a serial
 * connection.
 */
namespace serial {
    /**
     * Attempts to open a serial connection with the controller.
     * @return true if success
     */
    bool open(void);

    /**
     * Closes the serial connection to the controller.
     */
    void close(void);

    /**
     * Sends the controller a command to update the RGB LEDs to the given values.
     * @param r Red value, 0-255
     * @param g Green value, 0-255
     * @param b Blue value, 0-255
     */
    void sendColor(unsigned char r, unsigned char g, unsigned char b);
}

#endif // SERIAL_H
