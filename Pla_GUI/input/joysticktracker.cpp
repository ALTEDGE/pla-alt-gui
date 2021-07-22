#include "joysticktracker.h"
#include "config.h"

#include <cmath> // sqrt()
#include <iostream>

JoystickTracker::JoystickTracker(bool ts, bool ad) :
    KeySender(17),
    useSequencing(ts),
    useDiagonals(ad)
{

}

int JoystickTracker::toState(int v) const
{
    int s = 0;
    if (v > shortThreshold)
        s += (v > farThreshold) + 1;
    else if (v < -shortThreshold)
        s -= (v < -farThreshold) + 1;

    return s;
}

int JoystickTracker::getActionBits(int hstate, int vstate) const
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
    //if (bits & 0xFF00) {
    //    bits |= (bits << 8) & 0xFF00;
    //    bits &= ~(0xFF);
    //}

    return bits;
}

int JoystickTracker::getActionIndex(int hstate, int vstate) const
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

void JoystickTracker::dumpState(char id) const
{
    std::cout << "J" << id << ": (" << lastX << ", " << lastY << "): "
        << getActionIndex(toState(lastX), toState(lastY)) << std::endl;
}

void JoystickTracker::update(int x, int y, int pressed)
{
    // Calculate speed/distance traveled
    auto dx = x - lastX;
    auto dy = y - lastY;
    lastX = x;
    lastY = y;
    auto dist = std::sqrt(dx * dx + dy * dy);

    // Don't act if we're moving too quick (or are disabled)
    if (dist > config::JoystickSpeedThreshold || !isEnabled)
        return;

    // Get the current action index and fire that action
    // If the joystick is centered getActionIndex() will return -1,
    // and sendKey() will do nothing.
    auto vert = toState(y);
    auto horz = toState(x);

    if (useDiagonals) {
        Qt::KeyboardModifiers releasedMods = Qt::NoModifier;
        int bits = getActionBits(horz, vert);

        // Set bits get pressed, unset bits get released
        // Send releases first.
        for (int i = 0; i < 16; i++) {
            if (!(bits & (1 << i)))
                releasedMods |= sendKey(i, false);
        }
        for (int i = 0; i < 16; i++) {
            if (bits & (1 << i))
                sendKey(i, true, releasedMods);
        }
    } else {
        int index = getActionIndex(horz, vert);

        // Press index'th key, release all others
        for (int i = 0; i < 16; i++) {
            if (i != index)
                sendKey(i, false);
        }
        sendKey(index, true);
    }

    if (lastPressed != pressed) {
        if (!isButtonSticky) {
            sendKey(16, pressed);
        } else if (pressed) {
            stickyState ^= true;
            sendKey(16, stickyState);
        }
        lastPressed = pressed;
    }
}

void JoystickTracker::save(QSettings &settings) const
{
    KeySender::save(settings);
    saveThresholds(settings);

    settings.setValue("sequencer", useSequencing);
    settings.setValue("diagonals", useDiagonals);
    settings.setValue("sticky", isButtonSticky);
}

void JoystickTracker::load(QSettings &settings)
{
    KeySender::load(settings);
    loadThresholds(settings);

    useSequencing = settings.value("sequencer").toBool();
    useDiagonals = settings.value("diagonals").toBool();
    isButtonSticky = settings.value("sticky", false).toBool();
}
