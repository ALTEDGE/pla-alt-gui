#include "serial.h"

// Include platform-specific libraries
#ifdef _WIN64
#include <Windows.h>
#include <SetupAPI.h>
#else
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#endif

#include <iostream>
#include <string>

#ifdef _WIN64
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
#ifdef _WIN64
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
#ifdef _WIN64
    return hComPort != INVALID_HANDLE_VALUE;
#else
    return comFd != -1;
#endif // _WIN64
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

void Serial::nativeWrite(unsigned char *array, unsigned int count)
{
#ifdef _WIN64
    if (hComPort != INVALID_HANDLE_VALUE) {
        DWORD written = 0;
        WriteFile(hComPort, array, count, &written, nullptr);
    }
#else
    if (comFd != -1)
        ::write(comFd, array, count);
#endif
}

std::string Serial::nativeOpen(void)
{
#ifdef _WIN64
    // Load a handle to access all connected devices
    auto hDevInfo = SetupDiGetClassDevs(nullptr, nullptr, nullptr,
        DIGCF_ALLCLASSES | DIGCF_PRESENT);

    if (hDevInfo == INVALID_HANDLE_VALUE)
        return false;

    auto *devInfo = new SP_DEVINFO_DATA();
    std::string data (1024, '\0');
    int comPort = -1;

    std::memset(devInfo, 0, sizeof(SP_DEVINFO_DATA));
    devInfo->cbSize = sizeof(SP_DEVINFO_DATA);

    // Enumerate each device, to search for the connected controller
    DWORD index = 0;
    while (SetupDiEnumDeviceInfo(hDevInfo, index, devInfo)) {
        index++;

        auto getProperty = [&](DWORD property) {
            return SetupDiGetDeviceRegistryPropertyA(hDevInfo, devInfo,
                property, nullptr, reinterpret_cast<unsigned char *>(&data[0]),
                data.size(), nullptr);
        };

        // Attempt to load the device's hardware ID
        // and check if the ID matches the controller's
        // TODO ID is hard-coded...
        if (!getProperty(SPDRP_HARDWAREID) ||
            data.find("VID_1B4F&PID_9204") == std::string::npos)
            continue;

        // If we get to here, this should be our device.
        // Attempt to load the device's friendly name (should contain COM port number)
        if (!getProperty(SPDRP_FRIENDLYNAME))
            continue;

        // Look for the COM number
        auto comIndex = data.rfind("(COM");
        if (comIndex != std::string::npos) {
            // Device and port found
            comPort = std::stoi(data.substr(comIndex + 4));
            break;
        }
    }

    delete devInfo;

    if (comPort == -1)
        return "";

    // Attempt to open the serial port
    std::string comName ("\\\\.\\COM");
    comName += std::to_string(comPort);
    hComPort = CreateFileA(comName.data(), GENERIC_READ | GENERIC_WRITE, 0, nullptr,
        OPEN_EXISTING, FILE_FLAG_WRITE_THROUGH, nullptr);

    // TODO could check error with GetLastError()
    if (hComPort == INVALID_HANDLE_VALUE)
        return "";

    // Set connection parameters to what we need
    DCB params = { 0 };
    params.DCBlength = sizeof(DCB);
    GetCommState(hComPort, &params);
    params.BaudRate = CBR_9600; // 9600 baud
    params.ByteSize = 8;
    params.StopBits = ONESTOPBIT;
    params.Parity = ODDPARITY;
    SetCommState(hComPort, &params);

    // All set
    return comName;
#else
    char portBuffer[13] = "/dev/ttyACM\0";

    for (char i = '0'; i <= '9'; i++) {
        portBuffer[11] = i;
        comFd = ::open(portBuffer, O_RDWR | O_NOCTTY | O_SYNC);

        if (comFd != -1) {
            char buf[4];
            auto w = ::write(comFd, "i", 1);
            auto r = ::read(comFd, buf, 4);
            if (w == 1 && r == 4 && strncmp(buf, "PLA\n", 4) == 0)
                break;
            else
                ::close(comFd);
        }
    }

    return connected() ? portBuffer : "";
#endif

}
