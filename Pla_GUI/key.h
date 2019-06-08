#ifndef KEY_H
#define KEY_H

#include <QKeySequence>
#include <QSettings>

#include <array>

/**
 * @class Key
 * @brief Defines a key (with modifiers) that can be used as an action.
 */
class Key {
public:
    /**
     * Constructs a key with the given values.
     * @param k The key to use, using Qt's key values (e.g. Qt::Key_Down)
     * @param m Any modifiers to be pressed with the key
     */
    Key(int k = -1, Qt::KeyboardModifiers m = Qt::NoModifier);

    /**
     * Constructs a key for the given macro.
     * When this key is pressed, the macro will be fired.
     * @param macroName The macro to use
     */
    Key(const std::string& macroName);

    /**
     * Constructs a key from the given settings object.
     * The key's path in the settings should be set with QSettings.beginGroup()
     * before calling this constructor.
     * The key's value will be loaded from /key, and modifiers will be loaded from
     * /mod. Defaults to a key press.
     * @param settings The settings object to use
     */
    Key(const QSettings& settings);

    // "Equal" comparison, for Editing
    bool operator==(const Key& other) const;

    /**
     * Checks if this is a valid key, meaning that it represents a valid key.
     * @return True if this key is valid
     */
    bool isValid(void) const;

    /**
     * Returns a string to describe this key's value and modifiers.
     * Example output:
     *     "No binding"
     *     "Key: Shift + K"
     *     "Macro: My Macro"
     * @return The descriptive string
     */
    QString toString(void) const;

    /**
     * Sends a keystroke to the operating system based on this key's values.
     * @param press True for press, false for release
     */
    void fire(bool press) const;

    /**
     * Saves this key to the given settings object.
     * The key's path should be set via QSettings.beginGroup() beforehand.
     * @param settings The settings object to use
     */
    void save(QSettings& settings) const;

private:
    int key;
    Qt::KeyboardModifiers mod;

    std::string macro;
};



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

#endif // KEY_H
