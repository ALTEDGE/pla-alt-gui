#include "mainwindow.h"

#include "config.h"
#include "colortab.h"
#include "macrotab.h"
#include "profile.h"
#include "profiletab.h"
#include "programtab.h"
#include "wheeltab.h"

#include "controller.h"

#include <QApplication>
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
    // Keep the window at a fixed size.
    setWindowTitle("PLA ALT");
    setFixedSize(900, 588);
    installEventFilter(this);

    trayIcon = QSystemTrayIcon::isSystemTrayAvailable() ?
        new QSystemTrayIcon(QIcon("assets/icon.ico")) : nullptr;

    if (trayIcon) {
        // Create the context menu for the system tray icon
        auto systemTrayMenu = new QMenu();
        profileMenu = systemTrayMenu->addMenu("Set profile...");
        systemTrayMenu->addSeparator();
        auto quitAction = systemTrayMenu->addAction("Exit PLA ALT");

        connect(quitAction, SIGNAL(triggered(bool)), this, SLOT(handleQuit(bool)));

        profileActionGroup = new QActionGroup(profileMenu);
        connect(profileMenu, SIGNAL(aboutToShow()), this, SLOT(updateProfilesMenu()));

        // Prepare and show the icon
        connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(handleTray(QSystemTrayIcon::ActivationReason)));
        trayIcon->setContextMenu(systemTrayMenu);
        trayIcon->show();
    }

    lVersion.setAlignment(Qt::AlignRight);
    lVersion.setGeometry(0, 573, 900, 15);

    // Tab widget starts with a Y of 80px, so a banner can be at the top of the window
    tabs.setGeometry(0, 88, 900, 500);

    // Add tabs
    tabs.addTab(new ProfileTab(this), "PROFILES");
    tabs.addTab(new ColorTab(this), "LIGHTS");
    tabs.addTab(new ProgramTab(this), "PROGRAMMING");
    tabs.addTab(new WheelTab(this), "WHEEL");
    tabs.addTab(new MacroTab(this), "MACROS");

    connect(&tabs, SIGNAL(currentChanged(int)), this, SLOT(changeTab(int)));
    changeTab(0);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
#ifdef KEEP_OPEN_IN_TRAY
    if (!done) {
        if (trayIcon) {
            event->ignore();
            trayIcon->showMessage("PLA ALT Hidden", "Re-open PLA ALT by clicking the tray icon.",
                QSystemTrayIcon::Information, 4000);
            hide();
        } else {
            auto choice = QMessageBox::warning(this, "Exit PLA ALT",
                                               "Are you sure you want to quit?",
                                               QMessageBox::Ok | QMessageBox::Cancel);
            if (choice != QMessageBox::Ok)
                event->ignore();
        }
    } else {
#endif
        trayIcon->hide();
        event->accept();
#ifdef KEEP_OPEN_IN_TRAY
    }
#endif
}

bool MainWindow::eventFilter(QObject *, QEvent *event)
{
    switch (event->type()) {
    case QEvent::WindowActivate:
        Controller::setEnabled(false);
        break;
    case QEvent::WindowDeactivate:
        if (QApplication::activeWindow() == nullptr)
            Controller::setEnabled(true);
        break;
    default:
        break;
    }

    return false;
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
