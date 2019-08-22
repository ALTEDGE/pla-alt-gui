/**
 * @file profiletab.h
 * @brief Code for the Profile tab
 */
#ifndef PROFILETAB_H
#define PROFILETAB_H

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QShowEvent>
#include <QWidget>

/**
 * @class ProfileTab
 * @brief Provides controls for loading and managing profiles.
 */
class ProfileTab : public QWidget
{
    Q_OBJECT
public:
    explicit ProfileTab(QWidget *parent = nullptr);

private slots:
    void openProfile(const QString& name);
    void saveProfile(void);
    void deleteProfile(void);
    void newProfile(void);
    void renameProfile(void);

private:
    void showEvent(QShowEvent *event);

    QLabel lCurrentProfile;
    QLabel lProfileName;

    QComboBox profiles;
    QLineEdit profileName;

    QPushButton profileDelete;
    QPushButton profileNew;
    QPushButton profileRename;
    QPushButton profileSave;
};

#endif // PROFILETAB_H
