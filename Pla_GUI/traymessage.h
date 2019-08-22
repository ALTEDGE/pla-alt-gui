/**
 * @file traymessage.h
 * @brief Utility for creating notifications.
 */
#ifndef TRAYMESSAGE_H_
#define TRAYMESSAGE_H_

#include <QThread>

#include "mainwindow.h"

/**
 * @class TrayMessage
 * @brief Allows threads to post messages from the tray icon.
 */
class TrayMessage : public QThread
{
    Q_OBJECT

public:
    /**
     * Shows tray message with the given title and message.
     */
    void show(QString _title, QString _msg) {
        title = _title;
        msg = _msg;
        start();
    }

private:
    /**
     * Function called by QThread when start() is called.
     */
    void run(void) override {
        auto* icon = MainWindow::getTrayIcon();

        if (icon != nullptr) {
            QMetaObject::invokeMethod(icon, "showMessage", Qt::QueuedConnection,
                Q_ARG(QString, title),
                Q_ARG(QString, msg));
        }
    }

    QString title;
    QString msg;
};


#endif // TRAYMESSAGE_H_
