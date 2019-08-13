#include "mainwindow.h"

#include "config.h"
#include "colortab.h"
#include "macrotab.h"
#include "profile.h"
#include "profiletab.h"
#include "programtab.h"
#include "wheeltab.h"

#include "controller.h"

#include <QMessageBox>

QSystemTrayIcon *MainWindow::trayIcon;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    tabs(this),
    profileMenu(nullptr),
    profileActionGroup(nullptr),
    lVersion(config::versionString, this),
    lastTabIndex(0),
    done(false)
{
    trayIcon = new QSystemTrayIcon(QIcon("icon.png"));

    // Keep the window at a fixed size.
    setFixedSize(640, 490);

    // Create the context menu for the system tray icon
    auto systemTrayMenu = new QMenu();
    profileMenu = systemTrayMenu->addMenu("Set profile...");
    systemTrayMenu->addSeparator();
    auto quitAction = systemTrayMenu->addAction("Exit Pla GUI");

    connect(quitAction, SIGNAL(triggered(bool)), this, SLOT(handleQuit(bool)));

    profileActionGroup = new QActionGroup(profileMenu);
    connect(profileMenu, SIGNAL(aboutToShow()), this, SLOT(updateProfilesMenu()));

    // Prepare and show the icon
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
        this, SLOT(handleTray(QSystemTrayIcon::ActivationReason)));
    trayIcon->setContextMenu(systemTrayMenu);
    trayIcon->show();

    lVersion.setAlignment(Qt::AlignRight);
    lVersion.setGeometry(0, 480, 640, 10);

    // Tab widget starts with a Y of 80px, so a banner can be at the top of the window
    tabs.setGeometry(0, 80, 640, 400);

    // Add tabs
    tabs.addTab(new ProfileTab(this), "Profiles");
    tabs.addTab(new ColorTab(this), "LED Colors");
    tabs.addTab(new ProgramTab(this), "Programming");
    tabs.addTab(new WheelTab(this), "Wheel");
    tabs.addTab(new MacroTab(this), "Macros");

    connect(&tabs, SIGNAL(currentChanged(int)), this, SLOT(changeTab(int)));
    changeTab(0);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
#ifdef KEEP_OPEN_IN_TRAY
    if (!done) {
        event->ignore();
        trayIcon->showMessage("GUI Hidden", "Re-open PLA GUI by clicking the tray icon.",
            QSystemTrayIcon::Information, 4000);
        hide();
    } else {
#endif
        event->accept();
#ifdef KEEP_OPEN_IN_TRAY
    }
#endif
}

void MainWindow::changeTab(int index)
{
    if (index == lastTabIndex)
        return;

    // Check if tab is a SavableTab, if so, see if save prompt is needed
    auto tab = dynamic_cast<SavableTab*>(tabs.widget(lastTabIndex));
    if (tab != nullptr && tab->isModified()) {
        // Return focus to modified tab
        tabs.setCurrentWidget(dynamic_cast<QWidget*>(tab));

        auto choice = QMessageBox::warning(this, "Unsaved Changes",
            "Would you like to save your changes?", QMessageBox::Yes,
            QMessageBox::No, QMessageBox::Cancel);

        switch (choice) {
        case QMessageBox::Yes:
            // Save changes, switch to new tab
            tab->saveSettings();
            tabs.setCurrentIndex(index);
            break;
        case QMessageBox::No:
            // Discard changes, switch to new tab
            tab->loadSettings();
            tabs.setCurrentIndex(index);
            break;
        case QMessageBox::Cancel:
            return;
            break;
        default:
            break;
        }
    }

    lastTabIndex = index;
}

void MainWindow::handleQuit(bool unused)
{
    (void)unused;
    // Let closeEvent() know it's okay to exit
    done = true;
    emit exitingProgram();
    close();
}

void MainWindow::handleTray(QSystemTrayIcon::ActivationReason reason)
{
    // On left click, show the main window (this)
    if (reason == QSystemTrayIcon::ActivationReason::Trigger) {
        show();
        activateWindow();
    }
}

void MainWindow::updateProfilesMenu(void)
{
    profileActionGroup->actions().clear();
    profileMenu->clear();
    auto profiles = Profile::list();
    for (auto& p : profiles) {
        auto action = profileMenu->addAction(p);
        profileActionGroup->addAction(action);
        action->setCheckable(true);
        connect(action, SIGNAL(triggered(bool)), this, SLOT(loadProfile(bool)));
    }
}

void MainWindow::loadProfile(bool b)
{
    (void)b;
    for (auto& a : profileMenu->actions()) {
        if (a->isChecked()) {
            Profile::open(a->text());
            tabs.currentWidget()->setVisible(false);
            tabs.currentWidget()->setVisible(true);
            break;
        }
    }
}
