#include "thresholdsetter.h"

#include "controller.h"
#include "joysticktracker.h"
#include "profile.h"

#include <QtConcurrent/QtConcurrent>
#include <QPainter>

#include <chrono>
using namespace std::chrono_literals;

constexpr int mapSize = 90;

ThresholdSetter::ThresholdSetter(QWidget *parent) :
    QDialog(parent),
    name("PRIMARY"),
    lShortThresh("DEFAULT / VECTOR 1 THRESHOLD", this),
    lFarThresh("VECTOR 2 THRESHOLD", this),
    lCurrentPosition("PRIMARY POSITION", this),
    shortThreshold(this),
    farThreshold(this),
    //currentPrimary(this),
    configSave("SAVE", this),
    joyMap(mapSize, mapSize, QImage::Format_ARGB32),
    joyMapLabel(this),
    shouldUpdate(true)
{
    setWindowTitle("Threshold Settings");
    setFixedSize(180, 240);

    // Set geometries
    lCurrentPosition.setGeometry(0, 10, 180, 20);
    joyMapLabel.setGeometry(45, 30, mapSize, mapSize);
    lShortThresh.setGeometry(0, 130, 180, 20);
    shortThreshold.setGeometry(15, 150, 150, 10);
    lFarThresh.setGeometry(0, 170, 180, 20);
    farThreshold.setGeometry(15, 190, 150, 10);
    configSave.setGeometry(60, 210, 60, 20);

    lShortThresh.setAlignment(Qt::AlignCenter);
    lFarThresh.setAlignment(Qt::AlignCenter);
    lCurrentPosition.setAlignment(Qt::AlignCenter);
    shortThreshold.setOrientation(Qt::Horizontal);
    farThreshold.setOrientation(Qt::Horizontal);

    // Set slider ranges
    shortThreshold.setRange(100, 32767);
    farThreshold.setRange(100, 32767);

    connect(&configSave, SIGNAL(released()), this, SLOT(saveSettings()));
}

void ThresholdSetter::showEvent(QShowEvent *event)
{
    shouldUpdate = true;

    // Set sliders to current settings
    shortThreshold.setValue(Joystick::getShortThreshold());
    farThreshold.setValue(Joystick::getFarThreshold());

    // Start the joystick monitoring thread
    QtConcurrent::run([&](void) {
        static unsigned int counter = 0;
        while (shouldUpdate) {
            if (name[0] == 'L')
                joyPosition = Controller::Left.getPosition();
            else if (name[0] == 'R')
                joyPosition = Controller::Right.getPosition();
            else if (name[0] == 'P')
                joyPosition = Controller::Primary.getPosition();
            updateMap();
            QThread::msleep(100);

            if (++counter >= 10) {
                counter = 0;
                if (name[0] == 'L')
                    Controller::Left.dumpState(name.toStdString()[0]);
                else if (name[0] == 'R')
                    Controller::Right.dumpState(name.toStdString()[0]);
                else if (name[0] == 'P')
                    Controller::Primary.dumpState(name.toStdString()[0]);
            }
        }
    });

    event->accept();
}

void ThresholdSetter::closeEvent(QCloseEvent *event)
{
    // Bring back the monitor thread
    shouldUpdate = false;
    event->accept();
}

void ThresholdSetter::setJoystick(QString _name)
{
    name = _name;
    lCurrentPosition.setText(_name + " POSITION");
}


void ThresholdSetter::updateMap(void)
{
    QPainter pen (&joyMap);
    auto dist = sqrt(joyPosition.first * joyPosition.first +
        joyPosition.second * joyPosition.second);

    pen.fillRect(0, 0, mapSize, mapSize, dist > farThreshold.value() ? Qt::red : Qt::white);
    auto rs = shortThreshold.value() / 32767. * static_cast<double>(mapSize / 2.);
    auto rf = farThreshold.value() / 32767. * static_cast<double>(mapSize / 2.);
    pen.setPen(Qt::red);
    pen.setBrush(dist > shortThreshold.value() && dist < farThreshold.value() ?
        Qt::green : Qt::white);
    pen.drawEllipse(QPointF(mapSize / 2, mapSize / 2), rf, rf);
    pen.setPen(Qt::green);
    pen.setBrush(Qt::white);
    pen.drawEllipse(QPointF(mapSize / 2, mapSize / 2), rs, rs);

    pen.setPen(Qt::gray);
    pen.setBrush(Qt::gray);
    pen.drawLine(mapSize / 4, 0, mapSize * 3 / 4, mapSize);
    pen.drawLine(mapSize * 3 / 4, 0, mapSize / 4, mapSize);
    pen.drawLine(0, mapSize / 4, mapSize, mapSize * 3 / 4);
    pen.drawLine(0, mapSize * 3 / 4, mapSize, mapSize / 4);

    pen.setPen(Qt::black);
    pen.setBrush(Qt::black);
    pen.drawEllipse(QPointF((joyPosition.first + 32767) / 65536. * static_cast<double>(mapSize),
        mapSize - ((joyPosition.second + 32767) / 65536. * static_cast<double>(mapSize))), 3, 3);

    joyMapLabel.setPixmap(QPixmap::fromImage(joyMap));
}

void ThresholdSetter::saveSettings(void)
{
    Joystick::setShortThreshold(shortThreshold.value());
    Joystick::setFarThreshold(farThreshold.value());
    Joystick::saveThresholds(Profile::current());
    close();
}
