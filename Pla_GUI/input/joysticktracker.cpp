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

//void JoystickTracker::dumpState(char id) const
//{
//    std::cout << "J" << id << ": (" << lastX << ", " << lastY << "): "
//        << getActionIndex(toState(lastX), toState(lastY)) << std::endl;
//}

void JoystickTracker::update(int x, int y, int pressed)
{
    // 1. Update lastX/Y, and check travel speed.
    {
        if (!isEnabled) {
            lastX = x;
            lastY = y;
            return;
        }

        auto dx = x - lastX;
        auto dy = y - lastY;
        lastX = x;
        lastY = y;

        // Don't act if we're moving too quick (or are disabled)
        auto travelDistance = std::sqrt(dx * dx + dy * dy);
        if (travelDistance > config::JoystickSpeedThreshold)
            return;
    }

    // 2. Convert joystick position to action positions.
    //    These range -2 to 2: +-2 for far threshold,
    //    +-1 for short, 0 for no action.
    int vert, horz;
    auto dist = std::min(std::sqrt(x * x + y * y), 32767.);
    if (dist < shortThreshold) {
        vert = 0;
        horz = 0;
    } else {
        int mult = dist < farThreshold ? 1 : 2;
        auto ang = std::atan2(y, x);

        vert = mult * std::round(std::sin(ang));
        horz = mult * std::round(std::cos(ang));
    }

    // 3. Use action positions to determine presses and releases.
    if (useDiagonals) {
        int bits = getActionBits(horz, vert);

        // Set bits get pressed, unset bits get released
        for (int i = 0; i < 16; i++) {
            if (bits & (1 << i))
                sendKey(i, true);
        }
        for (int i = 0; i < 16; i++) {
            if (!(bits & (1 << i)))
                sendKey(i, false);
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

    useSequencing = settings.value("sequencer", false).toBool();
    useDiagonals = settings.value("diagonals", false).toBool();
    isButtonSticky = settings.value("sticky", false).toBool();
}
