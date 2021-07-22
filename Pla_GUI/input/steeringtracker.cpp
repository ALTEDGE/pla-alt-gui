#include "steeringtracker.h"

SteeringTracker::SteeringTracker(bool d) :
    KeySender(2),
    digital(d)
{

}

void SteeringTracker::setDigital(bool v)
{
    digital = v;
}


bool SteeringTracker::getDigital(void) const
{
    return digital;
}

void SteeringTracker::update(int pos) {
    lastPosition = pos;

    if (!digital || !isEnabled)
        return;

    if (pos > shortThreshold) {
        sendKey(1, true); // right
    } else if (pos < -shortThreshold) {
        sendKey(0, true); // left
    } else {
        sendKey(0, false);
        sendKey(1, false);
    }
}

void SteeringTracker::save(QSettings &settings) const
{
    settings.setValue("digital", digital);
    KeySender::save(settings);
    saveThresholds(settings);
}

void SteeringTracker::load(QSettings &settings)
{
    digital = settings.value("digital").toBool();
    KeySender::load(settings);
    loadThresholds(settings);
}
