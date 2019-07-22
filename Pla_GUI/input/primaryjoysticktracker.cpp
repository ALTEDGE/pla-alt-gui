#include "primaryjoysticktracker.h"

PrimaryJoystickTracker::PrimaryJoystickTracker(bool s, bool d) :
    JoystickTracker(s, d)
{
    setPG(0);
}

void PrimaryJoystickTracker::setPGKey(int pg, int index, Key key)
{
    //static_assert (pg < 8 && pg >= 0, "PG bounds check");
    //static_assert (index < 16 && index >= 0, "Index bounds check");

    // Validate arguments
    if (pg >= 8 || index >= 16)
        return;

    groups[pg][index] = key;

    // Update the base class' (KeySender) keys
    if (pg == currentPG)
        keys[index].first = groups[pg][index];
}

void PrimaryJoystickTracker::setPG(int pg)
{
    if (pg >= 8)
        return;

    currentPG = pg;

    // Update the base class' (KeySender) keys
    for (unsigned int i = 0; i < 16; i++)
        keys[i].first = groups[pg][i];
}

QString PrimaryJoystickTracker::getText(int pg, int index) const
{
    if (pg >= 8 || index >= 16)
        return QString();

    return groups[pg][index].toString();
}

void PrimaryJoystickTracker::save(QSettings& settings) const
{
    JoystickTracker::save(settings);

    // For each PG
    for (unsigned int i = 0; i < 8; i++) {
        settings.beginGroup("pg" + QString::fromStdString(std::to_string(i)));

        // For each action
        for (unsigned int j = 0; j < 16; j++) {
            settings.beginGroup(QString::fromStdString(std::to_string(j)));
            groups[i][j].save(settings);
            settings.endGroup();
        }

        settings.endGroup();
    }
}

void PrimaryJoystickTracker::load(QSettings& settings)
{
    JoystickTracker::load(settings);

    // For each PG
    for (unsigned int i = 0; i < 8; i++) {
        settings.beginGroup("pg" + QString::fromStdString(std::to_string(i)));

        // For each action
        for (unsigned int j = 0; j < 16; j++) {
            settings.beginGroup(QString::fromStdString(std::to_string(j)));
            groups[i][j] = Key(settings);
            settings.endGroup();
        }

        settings.endGroup();
    }

    setPG(currentPG);
}
