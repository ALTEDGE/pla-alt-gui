#include "primaryjoysticktracker.h"

PrimaryJoystickTracker::PrimaryJoystickTracker() :
    currentPG(0) {}

JoystickTracker& PrimaryJoystickTracker::getPG(int pg)
{
    if (pg == -1)
        pg = currentPG.load();
    return groups.at(pg);
}

void PrimaryJoystickTracker::setPG(int pg)
{
    if (pg >= 0 && pg < 8)
        currentPG.store(pg);
}

void PrimaryJoystickTracker::save(QSettings& settings) const
{
    // For each PG
    for (unsigned int i = 0; i < 8; i++) {
        settings.beginGroup(QString("pg%1").arg(i));
        groups[i].save(settings);
        settings.endGroup();
    }
}

void PrimaryJoystickTracker::load(QSettings& settings)
{
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

