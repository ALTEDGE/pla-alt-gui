#include "config.h"
#include "joystick.h"

int Joystick::shortThreshold = config::JoystickDefaultShortThreshold;
int Joystick::farThreshold = config::JoystickDefaultFarThreshold;

int Joystick::getShortThreshold(void)
{
    return shortThreshold;
}

int Joystick::getFarThreshold(void)
{
    return farThreshold;
}

void Joystick::setShortThreshold(int value)
{
    shortThreshold = value;
}

void Joystick::setFarThreshold(int value)
{
    farThreshold = value;
}

void Joystick::loadThresholds(QSettings& config)
{
    config.beginGroup("thresholds");
    shortThreshold = config.value("short", config::JoystickDefaultShortThreshold).toInt();
    farThreshold = config.value("far", config::JoystickDefaultFarThreshold).toInt();
    config.endGroup();
}

void Joystick::saveThresholds(QSettings& config)
{
    config.beginGroup("thresholds");
    config.setValue("short", shortThreshold);
    config.setValue("far", farThreshold);
    config.endGroup();
}

