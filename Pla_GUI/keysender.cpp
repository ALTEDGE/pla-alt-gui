#include "keysender.h"

std::vector<std::pair<Key, int>> KeySender::pressedKeys;

KeySender::KeySender(unsigned int count) :
    keys(count, {Key(), false}) {}

Qt::KeyboardModifiers KeySender::sendKey(int index, bool press, Qt::KeyboardModifiers mods)
{
    if (index < 0 || index >= static_cast<int>(keys.size()))
        return Qt::NoModifier;
    if (keys[index].second == press && (keys[index].first.getModifiers() & mods) == 0)
        return Qt::NoModifier;

    keys[index].second = press;
    const auto& key = keys[index].first;

    if (press) {
        // Only send keystroke if this is the first press of it
        for (auto& k : pressedKeys) {
            if (k.first == key) {
                //++k.second;
                return Qt::NoModifier;
            }
        }
        // First press:
        pressedKeys.emplace_back(key, 1);
        key.fire(press);
        return key.getModifiers();
    } else {
        // Check other keys, only release if last instance of the keystroke.
        for (auto k = pressedKeys.begin();
             k != pressedKeys.end();
             ++k)
        {
            if (k->first == key) {
                if (--k->second == 0) {
                    pressedKeys.erase(k);
                    key.fire(press);
                    return key.getModifiers();
                }
                break;
            }
        }

        return Qt::NoModifier;
    }
}

void KeySender::noPressedKeys()
{
    if (pressedKeys.size() > 0) {
        for (auto& k : pressedKeys)
            k.first.fire(false);
        pressedKeys.clear();
    }
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

