#ifndef TRAYMESSAGE_H_
#define TRAYMESSAGE_H_

#include <QThread>

#include "mainwindow.h"

class TrayMessage : public QThread
{
    Q_OBJECT

public:
    void show(QString _title, QString _msg) {
        title = _title;
        msg = _msg;
        start();
    }

private:
    void run(void) override {
        MainWindow::getTrayIcon().showMessage(title, msg,
            QSystemTrayIcon::Information, 1000);
    }

    QString title;
    QString msg;
};


#endif // TRAYMESSAGE_H_
