#include "steeringtracker.h"

SteeringTracker::SteeringTracker(bool d) :
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
    if (!digital)
        return;

    // Quirky steering wheel returns 32767 if the wire is loose
    if (pos > shortThreshold && pos != 32767) {
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
    settings.setValue("steering/digital", digital);
    KeySender::save(settings);
}

void SteeringTracker::load(QSettings &settings)
{
    digital = settings.value("steering/digital").toBool();
    KeySender::load(settings);
}
