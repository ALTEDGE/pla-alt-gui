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

    if (pos > shortThreshold * 1.025) {
        sendKey(1, true); // right
    } else if (pos < -shortThreshold * 1.025) {
        sendKey(0, true); // left
    } else if (std::abs(pos) < shortThreshold * 0.975) {
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
    digital = settings.value("digital", false).toBool();
    KeySender::load(settings);
    loadThresholds(settings);
}
