#include "profile.h"
#include "controller.h"
#include "macro.h"
#include "serial.h"

#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

static const QString profileFolderPath = (QStandardPaths::writableLocation(QStandardPaths::StandardLocation::ConfigLocation) + "/PLA/profiles/");
static const QString profileExtension (".ini");

static Profile profileInstance;

QString profileFile(const QString& name)
{
    return name + profileExtension;
}

QString profilePath(const QString& name)
{
    return profileFolderPath + profileFile(name);
}

QSettings* profileObject(const QString& name)
{
    return new QSettings(profilePath(name), QSettings::IniFormat);
}

QSettings *Profile::settings = nullptr;
QString Profile::settingsName;

Profile *Profile::instance()
{
    return &profileInstance;
}

void Profile::emitProfileChanged()
{
    emit profileChanged();
}

void Profile::open(const QString &name)
{
    if (name == settingsName)
        return;

    if (settings != nullptr) {
        settings->sync();
        delete settings;
    }

    QFile file (profilePath(name));
    bool newProfile = !file.exists();
    if (newProfile) {
        file.open(QFile::WriteOnly);
        file.write("\n");
        file.close();
    }

    settingsName = name;
    settings = profileObject(settingsName);

    Controller::load(*settings);
    Macro::load(*settings);

    if (newProfile)
        save();

    profileInstance.emitProfileChanged();
}

void Profile::openFirst(void)
{
    auto profiles = list();
    open(profiles.empty() ? "Profile 1" : profiles[0]);
}

void Profile::remove(void)
{
    auto profiles = list();
    if (profiles.count() == 1 && profiles[0] == "Profile 1")
        return;

    delete settings;
    settings = nullptr;

    QFile file (profilePath(settingsName));
    file.remove();

    openFirst();
}

void Profile::rename(const QString &newName)
{
    if (settingsName == newName)
        return;

    settings->sync();
    delete settings;
    settings = nullptr;

    QFile file (profilePath(settingsName));
    file.rename(profilePath(newName));
    open(newName);
}

void Profile::copy(const QString& newName)
{
    auto oldPath = profilePath(settingsName);

    {
        QFile file (oldPath);
        file.copy(oldPath + ".bak");
    }

    rename(newName);

    QFile oldFile (oldPath + ".bak");
    oldFile.rename(oldPath);
}

void Profile::save(void)
{
    settings->sync();
}

QSettings& Profile::current(void)
{
    return *settings;
}

const QString& Profile::name(void)
{
    return settingsName;
}

QStringList Profile::list(void)
{
    QDir profiles (profileFolderPath);
    QStringList names;

    if (!profiles.exists())
        profiles.mkpath(".");

    auto list = profiles.entryInfoList();
    for (QFileInfo file : list) {
        if (file.isFile())
            names.append(file.baseName());
    }
    return names;
}
