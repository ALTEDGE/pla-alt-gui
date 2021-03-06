#include "wheelthresholdsetter.h"

#include "controller.h"
#include "joysticktracker.h"
#include "profile.h"

#include <QApplication>
#include <QtConcurrent/QtConcurrent>
#include <QPainter>

#include <chrono>
using namespace std::chrono_literals;

WheelThresholdSetter::WheelThresholdSetter(QWidget *parent, QWidget *mainwindow) :
    QDialog(parent),
    lMap("WHEEL POSITION", this),
    lThresh("WHEEL THRESHOLD", this),
    threshold(this),
    configSave("SAVE", this),
    map(220, 30, QImage::Format_ARGB32),
    mapLabel(this),
    shouldUpdate(true)
{
    setWindowTitle("Trigger Settings");
    setFixedSize(240, 150);
    setModal(true);

    // Set geometries
    lMap.setGeometry(0, 10, 240, 20);
    lMap.setAlignment(Qt::AlignCenter);
    mapLabel.setGeometry(8, 30, 224, 30);
    mapLabel.setStyleSheet("border: 2px solid white");
    lThresh.setGeometry(0, 70, 240, 20);
    lThresh.setAlignment(Qt::AlignCenter);
    threshold.setGeometry(10, 90, 220, 10);
    configSave.setGeometry(90, 120, 60, 20);

    threshold.setOrientation(Qt::Horizontal);
    threshold.setRange(3000, 30000);

    connect(&configSave, SIGNAL(released()), this, SLOT(saveSettings()));
    connect(mainwindow, SIGNAL(exitingProgram()), this, SLOT(close()));
}

void WheelThresholdSetter::showEvent(QShowEvent *event)
{
    shouldUpdate = true;

    // Set sliders to current settings
    threshold.setValue(Controller::Steering.getShortThreshold());

    // Start the joystick monitoring thread
    installEventFilter(this);
    joyThread = QtConcurrent::run([&] {
        Controller::setEnabled(true);
        Controller::setOperating(false);
        while (shouldUpdate) {
            position = Controller::Steering.getPosition();
            updateMap();
            QThread::msleep(100);
        }
        Controller::setOperating(true);
        Controller::setEnabled(false);
        return 0;
    });

    if (event != nullptr)
        event->accept();
}

void WheelThresholdSetter::hideEvent(QHideEvent *event)
{
    // Bring back the monitor thread
    shouldUpdate = false;
    joyThread.result();
    removeEventFilter(this);
    if (event != nullptr)
        event->accept();
}

bool WheelThresholdSetter::eventFilter(QObject *, QEvent *event)
{
    switch (event->type()) {
    case QEvent::WindowActivate:
        Controller::setOperating(false);
        break;
    case QEvent::WindowDeactivate:
        if (QApplication::activeWindow() == nullptr)
            Controller::setOperating(true);
        break;
    default:
        break;
    }

    return false;
}

void WheelThresholdSetter::updateMap(void)
{
    QPainter pen (&map);

    // Background
    pen.fillRect(0, 0, 220, 30, abs(position) > threshold.value() ? Qt::green : Qt::black);
    // Threshold
    pen.setPen([]{QPen pen; pen.setWidth(2); pen.setColor(Qt::green); return pen; }());
    pen.setBrush(Qt::black);
    int width = static_cast<int>(threshold.value() / 32767. * 220);
    pen.drawRect(109 - width / 2, -1, width + 1, 30);
    // Position
    pen.setPen(Qt::white);
    pen.setBrush(Qt::white);
    int x = std::min(std::max(109 + static_cast<int>(position / 32767. * 110), 0), 220);
    pen.drawRect(x - 1, 4, 2, 21);

    mapLabel.setPixmap(QPixmap::fromImage(map));
}

void WheelThresholdSetter::saveSettings(void)
{
    Controller::Steering.setShortThreshold(threshold.value());
    Controller::save(Profile::current());
    Profile::save();
    close();
}
