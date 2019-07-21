#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCloseEvent>
#include <QMainWindow>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QTabWidget>

/**
 * @class MainWindow
 * @brief The main window, contains all controls/content
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    static inline QSystemTrayIcon* getTrayIcon(void) {
        return trayIcon;
    }

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

    static QSystemTrayIcon *trayIcon;

    QTabWidget tabs;
    QMenu *profileMenu;
    QActionGroup *profileActionGroup;

    int lastTabIndex;
    bool done;
};

#endif // MAINWINDOW_H
