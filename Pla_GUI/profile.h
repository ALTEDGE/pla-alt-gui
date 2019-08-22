/**
 * @file profile.h
 * @brief Controller profile management.
 */
#ifndef PROFILE_H
#define PROFILE_H

#include <QSettings>

/**
 * @class Profile
 * @brief Manages profiles that contain joystick configurations.
 */
class Profile
{
public:
    /**
     * Opens the profile with the given name, creating it if it doesn't exist.
     * @param name The name of the desired profile
     */
    static void open(const QString& name);
    /**
     * Opens the first profile in the profile directory (list is alphabetical).
     */
    static void openFirst(void);

    /**
     * Removes the currently open profile, permanently deleting it.
     */
    static void remove(void);

    /**
     * Renames the current profile to the given name. Overwrites existing profiles.
     * @param newName The new name for the profile
     */
    static void rename(const QString& newName);

    /**
     * Copies (duplicates) the current profile to the given name.
     * @param newName The name to copy this profile to
     */
    static void copy(const QString& newName);

    /**
     * Saves the current profile to it's file.
     */
    static void save(void);

    /**
     * Gets the current profile's settings object.
     * Profile values may be retrieved or set through this object.
     */
    static QSettings& current(void);

    /**
     * Gets the name of the current profile.
     */
    static const QString& name(void);

    /**
     * Gets the list of saved profiles.
     */
    static QStringList list(void);

private:
    static QSettings *settings;
    static QString settingsName;
};

#endif // PROFILE_H
