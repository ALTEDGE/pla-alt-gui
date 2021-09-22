#include "serial.h"
#include "config.h"

// Include platform-specific libraries
#ifdef PLA_WINDOWS
#include <Windows.h>
#include <SetupAPI.h>
#include <cstring>
#else
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#endif

#include <iostream>
#include <string>

#ifdef PLA_WINDOWS
HANDLE Serial::hComPort = INVALID_HANDLE_VALUE;
#else
int Serial::comFd = -1;
#endif

unsigned char Serial::colorBuffer[4] = "c";

bool Serial::open(void)
{
    auto port = nativeOpen();
    if (!port.empty()) {
        std::cout << "Controller on " << port << std::endl;
        return true;
    }

    return false;
}

void Serial::close(void)
{
#ifdef PLA_WINDOWS
    // Release the COM port
    if (hComPort != INVALID_HANDLE_VALUE) {
        CloseHandle(hComPort);
        hComPort = INVALID_HANDLE_VALUE;
    }
#else
    if (comFd != -1) {
        ::close(comFd);
        comFd = -1;
    }
#endif
}

bool Serial::connected(void)
{
#ifdef PLA_WINDOWS
    return hComPort != INVALID_HANDLE_VALUE;
#else
    return comFd != -1;
#endif // PLA_WINDOWS
}

void Serial::sendColor(unsigned char r, unsigned char g, unsigned char b)
{
    // A single 'c' character puts the controller into a color-receiving state
    // Expects a byte of red, green, and blue each
    // (500ms timeout to send the data)
    colorBuffer[1] = r;
    colorBuffer[2] = g;
    colorBuffer[3] = b;

    sendColor();
}

void Serial::sendColor(void)
{
    nativeWrite(colorBuffer, 4);
}

void Serial::sendLights(bool on)
{
    unsigned char code = on ? 'e' : 'd';
    nativeWrite(&code, 1);
}

int Serial::getPg()
{
    unsigned char code = 'p';
    nativeWrite(&code, 1);
    unsigned char buf = 0;
    nativeRead(&buf, 1);
    return buf;
}

void Serial::setPg(unsigned int pg)
{
    unsigned char buf[2] = {
        'P',
        static_cast<unsigned char>(pg)
    };
    nativeWrite(buf, sizeof(buf));
}

void Serial::nativeWrite(unsigned char *array, unsigned int count)
{
#ifdef PLA_WINDOWS
    if (hComPort != INVALID_HANDLE_VALUE) {
        DWORD written = 0;
        WriteFile(hComPort, array, count, &written, nullptr);
    }
#else
    if (comFd != -1)
        ::write(comFd, array, count);
#endif
}

void Serial::nativeRead(unsigned char *array, unsigned int count)
{
#ifdef PLA_WINDOWS
    if (hComPort != INVALID_HANDLE_VALUE) {
        DWORD read = 0;
        ReadFile(hComPort, array, count, &read, nullptr);
    }
#else
    if (comFd != -1)
        ::read(comFd, array, count);
#endif
}

std::string Serial::nativeOpen(void)
{
#ifdef PLA_WINDOWS
    std::string comName;

    // Load a handle to access all connected devices
    auto hDevInfo = SetupDiGetClassDevs(nullptr, nullptr, nullptr,
        DIGCF_ALLCLASSES | DIGCF_PRESENT);

    if (hDevInfo == INVALID_HANDLE_VALUE)
        return "";

    auto *devInfo = new SP_DEVINFO_DATA();
    std::string data (1024, '\0');

    std::memset(devInfo, 0, sizeof(SP_DEVINFO_DATA));
    devInfo->cbSize = sizeof(SP_DEVINFO_DATA);

    // Enumerate each device, to search for the connected controller
    DWORD index = 0;
    while (SetupDiEnumDeviceInfo(hDevInfo, index, devInfo)) {
        index++;

        auto getProperty = [&](DWORD property) {
            return SetupDiGetDeviceRegistryPropertyA(hDevInfo, devInfo,
                property, nullptr, reinterpret_cast<unsigned char *>(&data[0]),
                static_cast<DWORD>(data.size()), nullptr);
        };

        // Attempt to load the device's hardware ID
        // and check if the ID matches the controller's
        // TODO ID is hard-coded...
        if (!getProperty(SPDRP_HARDWAREID))
            continue;
        if (data.find(config::WindowsDeviceGUID[0]) == std::string::npos &&
            data.find(config::WindowsDeviceGUID[1]) == std::string::npos)
            continue;

        // If we get to here, this should be our device.
        // Attempt to load the device's friendly name (should contain COM port number)
        if (!getProperty(SPDRP_FRIENDLYNAME))
            continue;

        // Look for the COM number
        auto comIndex = data.rfind("(COM");
        if (comIndex != std::string::npos) {
            // Attempt to open the serial port
            ++comIndex;
            auto comIndexEnd = data.find(')', comIndex);
            comName = std::string("\\\\.\\") + data.substr(comIndex, comIndexEnd - comIndex);
            hComPort = CreateFileA(comName.data(), GENERIC_READ | GENERIC_WRITE,
                                   0, nullptr,
                                   OPEN_EXISTING, FILE_FLAG_WRITE_THROUGH, nullptr);

            // TODO could check error with GetLastError()
            if (hComPort != INVALID_HANDLE_VALUE) {
                // Set connection parameters to what we need
                DCB params {};
                params.DCBlength = sizeof(DCB);
                GetCommState(hComPort, &params);
                //params.BaudRate = CBR_9600; // Do not need to specify.
                params.ByteSize = 8;
                params.StopBits = ONESTOPBIT;
                params.Parity = ODDPARITY;
                SetCommState(hComPort, &params);

                // Send 'i' identification command
                unsigned char cmd = 'i';
                nativeWrite(&cmd, 1);
                char buf[3];
                nativeRead(reinterpret_cast<unsigned char *>(buf), 3);

                if (strncmp(buf, "PLA", 3) == 0) {
                    break;
                } else {
                    CloseHandle(hComPort);
                }
            }
        }
    }

    delete devInfo;
    return comName;
#else
    char portBuffer[13] = "/dev/ttyACM\0";

    for (char i = '0'; i <= '9'; i++) {
        portBuffer[11] = i;
        comFd = ::open(portBuffer, O_RDWR | O_NOCTTY | O_SYNC);

        if (comFd != -1) {
            char buf[3];
            auto w = ::write(comFd, "i", 1);
            auto r = ::read(comFd, buf, 3);
            if (w == 1 && r == 3 && strncmp(buf, "PLA", 3) == 0)
                break;
            else
                ::close(comFd);
        }
    }

    return connected() ? portBuffer : "";
#endif
}
