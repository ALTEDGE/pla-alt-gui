#include "wheelthresholdsetter.h"

#include "controller.h"
#include "joysticktracker.h"
#include "profile.h"

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
    setWindowTitle("Threshold Settings");
    setFixedSize(240, 150);

    // Set geometries
    lMap.setGeometry(0, 10, 240, 20);
    lMap.setAlignment(Qt::AlignCenter);
    mapLabel.setGeometry(10, 30, 220, 30);
    lThresh.setGeometry(0, 70, 240, 20);
    lThresh.setAlignment(Qt::AlignCenter);
    threshold.setGeometry(10, 90, 220, 10);
    configSave.setGeometry(90, 120, 60, 20);

    threshold.setOrientation(Qt::Horizontal);
    threshold.setRange(100, 32767);

    connect(&configSave, SIGNAL(released()), this, SLOT(saveSettings()));
    connect(mainwindow, SIGNAL(exitingProgram()), this, SLOT(close()));
}

void WheelThresholdSetter::showEvent(QShowEvent *event)
{
    shouldUpdate = true;

    // Set sliders to current settings
    threshold.setValue(Controller::Steering.getShortThreshold());

    // Start the joystick monitoring thread
    joyThread = QtConcurrent::run([&] {
        while (shouldUpdate) {
            position = Controller::Steering.getPosition();
            updateMap();
            QThread::msleep(100);
        }
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
    if (event != nullptr)
        event->accept();
}

void WheelThresholdSetter::updateMap(void)
{
    QPainter pen (&map);

    pen.fillRect(0, 0, 220, 30, abs(position) > threshold.value() ? Qt::green : Qt::white);
    pen.setPen(Qt::green);
    pen.setBrush(Qt::white);
    int width = static_cast<int>(threshold.value() / 32767. * 220);
    pen.drawRect(110 - width / 2, -1, width, 31);
    pen.setPen(Qt::black);
    pen.setBrush(Qt::black);
    int x = std::min(std::max(110 + static_cast<int>(position / 32767. * 110), 0), 220);
    pen.drawRect(x, 0, 1, 30);

    mapLabel.setPixmap(QPixmap::fromImage(map));
}

void WheelThresholdSetter::saveSettings(void)
{
    Controller::Steering.setShortThreshold(threshold.value());
    Controller::save(Profile::current());
    Profile::save();
    close();
}
