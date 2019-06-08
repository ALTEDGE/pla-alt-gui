#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCloseEvent>
#include <QMainWindow>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QTabWidget>

/**
 * Uncomment to let the program run in the background, minimized to the system
 * tray area on the start bar.
 */
#define KEEP_OPEN_IN_TRAY
 
/**
 * @class MainWindow
 * @brief The main window, contains all controls/content
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

private slots:
    /**
     * Prompts to save changes if tab is changed before saving.
     */
    void changeTab(int index);

    /**
     * Handles interaction with the system tray icon.
     */
    void handleTray(QSystemTrayIcon::ActivationReason);

    /**
     * Called through system tray, quits the program.
     * @param unused
     */
    void handleQuit(bool);

    void updateProfilesMenu(void);
    void loadProfile(bool);

private:
    /**
     * Closes the window, and exits the program if minimizing to the tray is
     * disabled.
     */
    void closeEvent(QCloseEvent *event);

    QSystemTrayIcon trayIcon;
    QTabWidget tabs;

    QMenu *profileMenu;
    QActionGroup *profileActionGroup;

    int lastTabIndex;
    bool done;
};

#endif // MAINWINDOW_H
