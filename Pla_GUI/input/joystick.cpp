#include "config.h"
#include "joystick.h"

int Joystick::getShortThreshold(void) const
{
    return shortThreshold;
}

int Joystick::getFarThreshold(void) const
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

void Joystick::saveThresholds(QSettings& config) const
{
    config.beginGroup("thresholds");
    config.setValue("short", shortThreshold);
    config.setValue("far", farThreshold);
    config.endGroup();
}

