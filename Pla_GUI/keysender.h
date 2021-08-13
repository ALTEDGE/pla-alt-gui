/**
 * @file keysender.h
 * @brief Handles action storage and firing.
 */
#ifndef KEYSENDER_H
#define KEYSENDER_H

#include "key.h"

#include <tuple>
#include <vector>

/**
 * @class KeySender
 * @brief Keeps a fixed number of keys that can be set or sent as keystrokes.
 * C defines how many keys the class should store.
 */
class KeySender {
public:
    KeySender(unsigned int count);

    /**
     * Sends the index'th key to the operating system as a keystroke.
     * Releases the previous key if one was pressed.
     * @param index The index of the key to send
     * @param press True for press, false for release
     */
    Qt::KeyboardModifiers sendKey(int index, bool press, Qt::KeyboardModifiers mods = Qt::NoModifier);

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

    const Key& getKey(int index) const {
        static Key dummy;
        if (index < 0 || index >= static_cast<int>(keys.size()))
            return dummy;
        return keys[index].first;
    }

    void noPressedKeys();

    /**
     * Returns a string to describe the index'th key.
     * @param index The index to read
     * @return A descriptive string
     */
    QString getText(int index) const;

    /**
     * Saves all keys to the given settings object.
     * @param settings The settings to modify
     */
    virtual void save(QSettings& settings) const;

    /**
     * Loads all keys from the given settings object.
     * @param settings The settings to read from
     */
    virtual void load(QSettings &settings);

    // For Editing
    bool operator==(const KeySender& other) const;

    // For Editing
    bool operator!=(const KeySender& other) const;

protected:
    // Stores values for the keys
    std::vector<std::pair<Key, bool>> keys;

private:
    static std::vector<std::pair<Key, int>> pressedKeys;
};

#endif // KEYSENDER_H
