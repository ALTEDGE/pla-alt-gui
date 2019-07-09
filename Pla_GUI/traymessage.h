#ifndef TRAYMESSAGE_H_
#define TRAYMESSAGE_H_

#include <QThread>

#include "mainwindow.h"

class TrayMessage : public QThread
{
    Q_OBJECT

    constexpr static unsigned int msecs = 1000;

public:
    void show(QString _title, QString _msg) {
        title = _title;
        msg = _msg;
        start();
    }

private:
    void run(void) override {
        auto* icon = MainWindow::getTrayIcon();
        if (icon != nullptr)
            icon->showMessage(title, msg, QSystemTrayIcon::Information, msecs);

    }

    QString title;
    QString msg;
};


#endif // TRAYMESSAGE_H_
