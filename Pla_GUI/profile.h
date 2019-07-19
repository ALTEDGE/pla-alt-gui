#ifndef PROFILE_H
#define PROFILE_H

#include <QSettings>

class Profile
{
public:
    static void open(const QString& name);
    static void openFirst(void);

    static void remove(void);
    static void rename(const QString& newName);
    static void copy(const QString& newName);

    static void save(void);

    static QSettings& current(void);
    static const QString& name(void);

    static QStringList list(void);

private:
    static QSettings *settings;
    static QString settingsName;
};

#endif // PROFILE_H
