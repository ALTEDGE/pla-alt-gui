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

namespace serial {

// Platform-specific variables
#ifdef _WIN64
static HANDLE hComPort = INVALID_HANDLE_VALUE;
#else
static int comFd = -1;
#endif

void write(unsigned char *array, unsigned int count);

bool open(void)
{
#ifdef _WIN64
    // Load a handle to access all connected devices
    auto hDevInfo = SetupDiGetClassDevs(nullptr, nullptr, nullptr, DIGCF_ALLCLASSES |
        DIGCF_PRESENT);

    if (hDevInfo == INVALID_HANDLE_VALUE)
        return false;

    int comPort = -1;
    DWORD index = 0;

    auto *devInfo = new SP_DEVINFO_DATA();
    ZeroMemory(devInfo, sizeof(SP_DEVINFO_DATA));
    devInfo->cbSize = sizeof(SP_DEVINFO_DATA);

    auto *data = new BYTE[1024];

    // Enumerate each device, to search for the connected controller
    while (SetupDiEnumDeviceInfo(hDevInfo, index, devInfo)) {
        index++;

        // Attempt to load the device's hardware ID
        if (!SetupDiGetDeviceRegistryPropertyA(hDevInfo, devInfo, SPDRP_HARDWAREID,
            nullptr, data, sizeof(BYTE) * 1024, nullptr))
            continue;

        // Check if the ID matches the controller's
        // TODO need to change if controller gets a unique ID (using Arduino's
        // for now)
        std::string hardwareId (reinterpret_cast<char *>(data));
        if (hardwareId.find("VID_1B4F&PID_9204") == std::string::npos)
            continue;

        // If we get to here, this should be our device.
        // Attempt to load the device's friendly name (should contain COM port number)
        if (!SetupDiGetDeviceRegistryPropertyA(hDevInfo, devInfo, SPDRP_FRIENDLYNAME,
            nullptr, data, 1024, nullptr))
            continue;

        // Look for the COM number
        std::string friendlyName (reinterpret_cast<char *>(data));
        auto comIndex = friendlyName.rfind("(COM");
        if (comIndex == std::string::npos)
            continue;

        // Device and port found
        comPort = std::stoi(friendlyName.substr(comIndex + 4));
        break;
    }
    delete[] data;
    delete devInfo;

    if (comPort == -1)
        return false;

    // Attempt to open the serial port
    std::wstring comName (L"\\\\.\\COM");
    comName.append(std::to_wstring(comPort));
    hComPort = CreateFile(comName.data(), GENERIC_READ | GENERIC_WRITE, 0, nullptr,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

    if (hComPort == INVALID_HANDLE_VALUE) {
        std::cout << "Failed to open serial connection (" << GetLastError() <<
            ")." << std::endl;
        return false;
    }

    // Set connection parameters to what we need
    DCB params = { 0 };
    params.DCBlength = sizeof(DCB);
    GetCommState(hComPort, &params);
    params.BaudRate = CBR_9600; // 9600 baud
    params.ByteSize = 8;
    params.StopBits = ONESTOPBIT;
    params.Parity = ODDPARITY;
    SetCommState(hComPort, &params);

    std::cout << "using " << comName.c_str() << std::endl;

    // All set
    return true;
#else
    char portBuffer[16];

    for (int i = 0; comFd == -1 && i < 8; i++) {
        snprintf(portBuffer, 16, "/dev/ttyACM%d", i);
        comFd = ::open(portBuffer, O_RDWR | O_NOCTTY);

        if (comFd == -1)
            continue;

        char buf[4];
        int w = ::write(comFd, "i", 1);
        int r = ::read(comFd, buf, 4);
        if (w == 1 && r == 4 && strncmp(buf, "PLA\n", 4)) {
            ::close(comFd);
            comFd = -1;
            continue;
        }
    }

    std::cout << "using " << portBuffer << std::endl;

    return comFd != -1;
#endif
}

void close(void)
{
#ifdef _WIN64
    // Release the COM port
    if (hComPort != INVALID_HANDLE_VALUE)
        CloseHandle(hComPort);
#else
    if (comFd != -1)
        ::close(comFd);
#endif
}

void sendColor(unsigned char r, unsigned char g, unsigned char b)
{
    // A single 'c' character puts the controller into a color-receiving state
    // Expects a byte of red, green, and blue each
    // (500ms timeout to send the data)
    unsigned char buf[4] = {
        'c', r, g, b
    };

    write(buf, 4);
}

void write(unsigned char *array, unsigned int count)
{
#ifdef _WIN64
    if (hComPort == INVALID_HANDLE_VALUE)
        return;

    DWORD written = 0;
    WriteFile(hComPort, array, count, &written, nullptr);
#else
    if (comFd == -1)
        return;

    ::write(comFd, array, count);
#endif
}

} // namespace serial
