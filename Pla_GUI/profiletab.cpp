#include "profiletab.h"
#include "mainwindow.h"
#include "profile.h"
#include "controller.h"
#include "macro.h"

#include <QDir>

ProfileTab::ProfileTab(QWidget *parent) :
    QWidget(parent),
    lCurrentProfile("CURRENT PROFILE", this),
    lProfileName("PROFILE NAME", this),
    profiles(this),
    profileName("", this),
    profileDelete("DELETE", this),
    profileNew("NEW", this),
    profileRename("RENAME", this),
    profileSave("SAVE", this),
    profileToPLA("SAVE TO CONTROLLER", this)
{
    lCurrentProfile.setGeometry(50, 30, 200, 20);
    profiles.setGeometry(50, 60, 200, 20);
    profileNew.setGeometry(50, 90, 60, 20);

    lProfileName.setGeometry(310, 30, 200, 20);
    profileName.setGeometry(310, 60, 200, 20);
    profileRename.setGeometry(520, 60, 60, 20);
    profileSave.setGeometry(325, 90, 80, 20);
    profileDelete.setGeometry(415, 90, 80, 20);
    profileToPLA.setGeometry(325, 120, 170, 20);

    profileToPLA.setVisible(false);

    lProfileName.setAlignment(Qt::AlignCenter);

    connect(&profiles, SIGNAL(currentTextChanged(QString)), this, SLOT(openProfile(QString)));
    connect(&profileSave, SIGNAL(released()), this, SLOT(saveProfile()));
    connect(&profileDelete, SIGNAL(released()), this, SLOT(deleteProfile()));
    connect(&profileNew, SIGNAL(released()), this, SLOT(newProfile()));
    connect(&profileRename, SIGNAL(released()), this, SLOT(renameProfile()));
}

void ProfileTab::showEvent(QShowEvent *event)
{
    auto profileNames = Profile::list();

    profiles.blockSignals(true);
    profiles.clear();
    for (const auto& name : profileNames)
        profiles.addItem(name);
    profiles.setCurrentText(Profile::name());
    profiles.blockSignals(false);

    profileName.setText(Profile::name());

    if (event != nullptr)
        event->accept();
}

void ProfileTab::openProfile(const QString &name)
{
    Profile::open(name);
    showEvent(nullptr);
}

void ProfileTab::saveProfile(void)
{
    auto& profile = Profile::current();
    Controller::save(profile);
    Macro::save(profile);

    if (Profile::name() != profileName.text()) {
        Profile::copy(profileName.text());
        Profile::save();
        showEvent(nullptr);
    } else {
        Profile::save();
    }
}

void ProfileTab::deleteProfile(void)
{
    Profile::remove();
    showEvent(nullptr);
}

void ProfileTab::newProfile(void)
{
    auto profileNames = Profile::list();

    int i;
    for (i = 1; profileNames.contains(QString("Profile %1").arg(i)); i++);
    openProfile(QString("Profile %1").arg(i));
}

void ProfileTab::renameProfile(void)
{
    Profile::rename(profileName.text());
    showEvent(nullptr);
}
