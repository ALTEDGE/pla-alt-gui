#include "joysticktracker.h"

#include <cmath> // sqrt()
#include <chrono>
#include <thread>

/**
 * Defines the minimum rate-of-change in a joystick to be considered movement.
 * Actions will not be fired unless the joystick is moving at a lesser speed.
 *
 * Note: Axes in SDL have a range of -32767 to +32767
 */
constexpr double speedThreshold = 100;

constexpr int defaultShortThreshold = 10000;
constexpr int defaultFarThreshold = 30000;

int Joystick::shortThreshold = defaultShortThreshold;
int Joystick::farThreshold = defaultFarThreshold;

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
    shortThreshold = config.value("short", defaultShortThreshold).toInt();
    farThreshold = config.value("far", defaultFarThreshold).toInt();
    config.endGroup();
}

void Joystick::saveThresholds(QSettings& config)
{
    config.beginGroup("thresholds");
    config.setValue("short", shortThreshold);
    config.setValue("far", farThreshold);
    config.endGroup();
}



//
// JoystickTracker code
//

JoystickTracker::JoystickTracker(bool ts, bool ad) :
    lastX(0),
    lastY(0),
    useSequencing(ts),
    useDiagonals(ad)
{

}

int JoystickTracker::toState(int v)
{
    int s = 0;
    if (v > shortThreshold)
        s += (v > farThreshold) + 1;
    else if (v < -shortThreshold)
        s -= (v < -farThreshold) + 1;

    return s;
}

int JoystickTracker::getActionBits(int hstate, int vstate)
{
    int bits = 0;

    // Set horizontal bits
    if (hstate > 0)
        bits |= (useSequencing && hstate == 2) ? (1 << 10) : (1 << 2);
    else if (hstate < 0)
        bits |= (useSequencing && hstate == -2) ? (1 << 14) : (1 << 6);

    // Set vertical bits
    if (vstate > 0)
        bits |= (useSequencing && vstate == 2) ? (1 << 8) : (1 << 0);
    else if (vstate < 0)
        bits |= (useSequencing && vstate == -2) ? (1 << 12) : (1 << 4);

    // some axis is at vector 2
    if (bits & 0xFF00) {
        bits |= (bits << 8) & 0xFF00;
        bits &= ~(0xFF);
    }

    return bits;
}

int JoystickTracker::getActionIndex(int hstate, int vstate)
{
    int action = -1;

    // Find horizontal positon
    if (hstate > 0) {
        action = 2;
        if (useSequencing)
            action += (hstate - 1) * 8;
    } else if (hstate < 0) {
        action = 6;
        if (useSequencing && hstate < -1)
            action += (hstate + 1) * -8;
    }

    // If +vertical
    if (vstate > 0) {
        // Handle far (second stage) position
        if (useSequencing && vstate == 2) {
            if (action == 10) // far left
                action--;
            else if (action == 14) // far right
                action++;
            else
                action = 8; // far up
        }
        // Handle close position
        else {
            if (action == 2) // left
                action--;
            else if (action == 6) // right
                action++;
            else
                action = 0;
        }
    }
    // If -vertical
    else if (vstate < 0) {
        // Handle far (second stage) position
        if (useSequencing && vstate == -2) {
            if (action == 10) // far left
                action++;
            else if (action == 14) // far right
                action--;
            else
                action = 12; // far down
        }
        // Handle close position
        else {
            if (action == 2) // left
                action++;
            else if (action == 6) // right
                action--;
            else
                action = 4;
        }
    }

    return action;
}

void JoystickTracker::setSequencing(bool enable)
{
    useSequencing = enable;
}

void JoystickTracker::setDiagonals(bool enable)
{
    useDiagonals = enable;
}

void JoystickTracker::update(int x, int y)
{
    // Calculate speed/distance traveled
    auto dx = x - lastX;
    auto dy = y - lastY;
    lastX = x;
    lastY = y;
    auto dist = std::sqrt(dx * dx + dy * dy);

    // Don't act if we're moving too quick
    if (dist > speedThreshold)
        return;

    // Get the current action index and fire that action
    // If the joystick is centered getActionIndex() will return -1,
    // and sendKey() will do nothing.
    auto vert = toState(y);
    auto horz = toState(x);

    if (useDiagonals) {
        int bits = getActionBits(horz, vert);

        // Set bits get pressed, unset bits get released
        for (int i = 0; i < 16; i++)
            sendKey(i, bits & (1 << i));
    } else {
        int index = getActionIndex(horz, vert);

        // Press index'th key, release all others
        for (int i = 0; i < 16; i++)
            sendKey(i, i == index);
    }
}

void JoystickTracker::save(QSettings &settings) const
{
    KeySender::save(settings);

    settings.setValue("sequencer", useSequencing);
    settings.setValue("diagonals", useDiagonals);
}

void JoystickTracker::load(QSettings &settings)
{
    KeySender::load(settings);

    useSequencing = settings.value("sequencer").toBool();
    useDiagonals = settings.value("diagonals").toBool();
}



//
// PrimaryJoystickTracker code
//

PrimaryJoystickTracker::PrimaryJoystickTracker(bool s, bool d) :
    JoystickTracker(s, d)
{
    setPG(0);
}

void PrimaryJoystickTracker::setPGKey(unsigned int pg, unsigned int index, Key key)
{
    // Validate arguments
    if (pg >= 8 || index >= 16)
        return;

    groups[pg][index] = key;

    // Update the base class' (KeySender) keys
    if (pg == currentPG)
        keys[index].first = groups[pg][index];
}

void PrimaryJoystickTracker::setPG(unsigned int pg)
{
    if (pg >= 8)
        return;

    currentPG = pg;

    // Update the base class' (KeySender) keys
    for (unsigned int i = 0; i < 16; i++)
        keys[i].first = groups[pg][i];
}

QString PrimaryJoystickTracker::getText(unsigned int pg, unsigned int index) const
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



//
// SteeringTracker code
//

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
