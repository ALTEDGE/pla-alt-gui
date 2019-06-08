#include "profile.h"
#include "controllerroutine.h"
#include "macro.h"
#include "serial.h"

#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

static const QString profileFolderPath = (QStandardPaths::writableLocation(QStandardPaths::StandardLocation::ConfigLocation) + "/PLA/profiles/");
static const QString profileExtension (".ini");

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

void Profile::open(const QString &name)
{
    if (name == settingsName)
        return;

    if (settings != nullptr) {
        settings->sync();
        delete settings;
    }

    QFile file (profilePath(name));
    if (!file.exists()) {
        file.open(QFile::WriteOnly);
        file.write("\n");
        file.close();
    }

    settingsName = name;
    settings = profileObject(settingsName);

    if (Controller::good()) {
        auto brght = settings->value("color/brightness", 20).toInt();
        auto red = settings->value("color/red", 255).toInt();
        auto green = settings->value("color/green", 0).toInt();
        auto blue = settings->value("color/blue", 0).toInt();
        serial::sendColor(red * brght / 100, green * brght / 100,
            blue  * brght / 100);
    }

    Controller::load(*settings);
    Macro::load(*settings);
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
    settings->sync();
    delete settings;
    settings = nullptr;

    QFile file (profilePath(settingsName));
    file.rename(profilePath(newName));
    open(newName);
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
