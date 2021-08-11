#include "primaryjoysticktracker.h"

PrimaryJoystickTracker::PrimaryJoystickTracker(bool s, bool d) :
    JoystickTracker(s, d)
{
    for (auto& g : groups)
        g = JoystickTracker(s, d);

    setPG(0);
}

void PrimaryJoystickTracker::saveCurrentPG()
{
    groups[currentPG] = *this;
}

void PrimaryJoystickTracker::setPG(int pg)
{
    if (pg >= 8)
        return;

    currentPG = pg;
    *dynamic_cast<JoystickTracker *>(this) = groups[pg];
}

void PrimaryJoystickTracker::save(QSettings& settings) const
{
    //JoystickTracker::save(settings);

    // For each PG
    for (unsigned int i = 0; i < 8; i++) {
        settings.beginGroup(QString("pg%1").arg(i));
        groups[i].save(settings);
        settings.endGroup();
    }
}

void PrimaryJoystickTracker::load(QSettings& settings)
{
    //JoystickTracker::load(settings);

    // For each PG
    for (unsigned int i = 0; i < 8; i++) {
        settings.beginGroup(QString("pg%1").arg(i));
        groups[i].load(settings);
        settings.endGroup();
    }

    setPG(currentPG);
}

bool PrimaryJoystickTracker::operator==(const PrimaryJoystickTracker& other)
{
    return groups == other.groups;
}

bool PrimaryJoystickTracker::operator!=(const PrimaryJoystickTracker& other)
{
    return groups != other.groups;
}

