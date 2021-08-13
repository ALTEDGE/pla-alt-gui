#include "keysender.h"

#include <iostream>

std::map<Qt::Key, int> KeySender::pressedKeys;

KeySender::KeySender(unsigned int count) :
    keys(count, {Key(), false}) {}

void KeySender::sendKey(int index, bool press)
{
    if (index < 0 || index >= static_cast<int>(keys.size()))
        return;
    if (keys[index].second == press)
        return;

    keys[index].second = press;

    auto tryKeyAction =
        [&](Qt::Key K) {
            if (!press)
                --pressedKeys[K];
            if (pressedKeys[K] == 0)
                Key(K).fire(press);
            if (press)
                ++pressedKeys[K];
        };

    const auto& key = keys[index].first;
    auto mods = key.getModifiers();
    if (mods & Qt::ShiftModifier)
        tryKeyAction(Qt::Key_Shift);
    if (mods & Qt::ControlModifier)
        tryKeyAction(Qt::Key_Control);
    if (mods & Qt::AltModifier)
        tryKeyAction(Qt::Key_Alt);
    if (mods & Qt::MetaModifier)
        tryKeyAction(Qt::Key_Meta);

    tryKeyAction(static_cast<Qt::Key>(key.getKey()));
}

QString KeySender::getText(int index) const
{
    if (index < 0 || index >= static_cast<int>(keys.size()))
        return "";
    return keys[index].first.toString();
}

void KeySender::save(QSettings& settings) const
{
    for (unsigned int i = 0; i < keys.size(); i++) {
        settings.beginGroup(QString::fromStdString(std::to_string(i)));
        keys[i].first.save(settings);
        settings.endGroup();
    }
}

void KeySender::load(QSettings &settings)
{
    for (unsigned int i = 0; i < keys.size(); i++) {
        settings.beginGroup(QString::fromStdString(std::to_string(i)));
        keys[i].first = Key(settings);
        settings.endGroup();
    }
}

bool KeySender::operator==(const KeySender& other) const
{
    if (keys.size() != other.keys.size())
        return false;

    for (auto i = 0u; i < keys.size(); ++i) {
        if (keys[i].first != other.keys[i].first)
            return false;
    }

    return true;
}

bool KeySender::operator!=(const KeySender& other) const
{
    return !(*this == other);
}

