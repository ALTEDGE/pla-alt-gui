#ifndef KEY_H
#define KEY_H

#include <QKeySequence>
#include <QSettings>

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

    inline Qt::KeyboardModifiers getModifiers(void) const {
        return mod;
    }

private:
    int key;
    Qt::KeyboardModifiers mod;

    std::string macro;
};

#endif // KEY_H
