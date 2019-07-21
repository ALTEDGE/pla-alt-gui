#ifndef KEYSENDER_H
#define KEYSENDER_H

#include "key.h"

#include <array>

/**
 * @class KeySender
 * @brief Keeps a fixed number of keys that can be set or sent as keystrokes.
 * C defines how many keys the class should store.
 */
template<unsigned int C>
class KeySender {
public:
    KeySender(void) {
        keys.fill(std::make_pair(Key(), false));
    }

    /**
     * Sends the index'th key to the operating system as a keystroke.
     * Releases the previous key if one was pressed.
     * @param index The index of the key to send, -1 to only release previous key
     */
    void sendKey(int index, bool press) {
        // Bound checks; don't allow consecutive presses/releases
        if (index < 0 || index >= static_cast<int>(keys.size()) ||
            press == keys[index].second)
            return;

        keys[index].first.fire(press);
        keys[index].second = press;
    }

    /**
     * Sets the index'th key, given arguments for one of Key's constructors.
     * @param index The index to set
     * @param args Arguments for Key's constructor
     */
    template<typename... Args>
    void setKey(int index, Args... args) {
        if (index < 0 || index >= static_cast<int>(keys.size()))
            return;

        keys[index].first = Key(args...);
    }

    /**
     * Returns a string to describe the index'th key.
     * @param index The index to read
     * @return A descriptive string
     */
    QString getText(unsigned int index) const {
        if (index >= keys.size())
            return QString();

        return keys[index].first.toString();
    }

    /**
     * Saves all keys to the given settings object.
     * @param settings The settings to modify
     */
    virtual void save(QSettings& settings) const {
        for (unsigned int i = 0; i < keys.size(); i++) {
            settings.beginGroup(QString::fromStdString(std::to_string(i)));
            keys[i].first.save(settings);
            settings.endGroup();
        }
    }

    /**
     * Loads all keys from the given settings object.
     * @param settings The settings to read from
     */
    virtual void load(QSettings &settings) {
        for (unsigned int i = 0; i < keys.size(); i++) {
            settings.beginGroup(QString::fromStdString(std::to_string(i)));
            keys[i].first = Key(settings);
            settings.endGroup();
        }
    }

    // For Editing
    bool operator==(const KeySender& other) {
        return keys == other.keys;
    }

    // For Editing
    bool operator!=(const KeySender& other) {
        return keys != other.keys;
    }

protected:
    // Stores values for the keys
    std::array<std::pair<Key, bool>, C> keys;

};

#endif // KEYSENDER_H