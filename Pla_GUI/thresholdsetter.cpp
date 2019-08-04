#include "thresholdsetter.h"

#include "controller.h"
#include "joysticktracker.h"
#include "profile.h"

#include <QtConcurrent/QtConcurrent>
#include <QPainter>

#include <chrono>
using namespace std::chrono_literals;

ThresholdSetter::ThresholdSetter(QWidget *parent) :
    QDialog(parent),
    lShortThresh("DEFAULT / VECTOR 1 THRESHOLD", this),
    lFarThresh("VECTOR 2 THRESHOLD", this),
    lCurrentPrimary("PRIMARY X+ CURRENT POSITION", this),
    shortThreshold(this),
    farThreshold(this),
    //currentPrimary(this),
    configSave("SAVE", this),
    joyMap(180, 180, QImage::Format_ARGB32),
    joyMapLabel(this),
    shouldUpdate(true)
{
    setWindowTitle("Threshold Settings");
    setFixedSize(240, 330);

    // Set geometries
    lShortThresh.setGeometry(0, 10, 240, 20);
    lFarThresh.setGeometry(0, 50, 240, 20);
    lCurrentPrimary.setGeometry(0, 90, 240, 20);
    shortThreshold.setGeometry(30, 30, 180, 10);
    farThreshold.setGeometry(30, 70, 180, 10);
    //currentPrimary.setGeometry(30, 110, 180, 10);
    configSave.setGeometry(90, 300, 60, 20);
    joyMapLabel.setGeometry(30, 110, 180, 180);

    lShortThresh.setAlignment(Qt::AlignCenter);
    lFarThresh.setAlignment(Qt::AlignCenter);
    lCurrentPrimary.setAlignment(Qt::AlignCenter);
    shortThreshold.setOrientation(Qt::Horizontal);
    farThreshold.setOrientation(Qt::Horizontal);
    //currentPrimary.setOrientation(Qt::Horizontal);

    // Set slider ranges
    shortThreshold.setRange(100, 32767);
    farThreshold.setRange(100, 32767);
    //currentPrimary.setRange(100, 32767);

    //connect(&shortThreshold, SIGNAL(sliderMoved(int)), this, SLOT(repaint()));
    //connect(&farThreshold, SIGNAL(sliderMoved(int)), this, SLOT(repaint()));
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
        while (shouldUpdate) {
            joyPosition = Controller::Primary.getPosition();
            updateMap();
            QThread::msleep(100);
        }
    });

    event->accept();
}

void ThresholdSetter::closeEvent(QCloseEvent *event)
{
    // Bring back the monitor thread
    shouldUpdate = false;
    updateCurrent.join();

    event->accept();
}

void ThresholdSetter::updateMap(void)
{
    QPainter pen (&joyMap);

    pen.fillRect(0, 0, 180, 180, Qt::white);
    auto rs = shortThreshold.value() / 32767. * 90.;
    auto rf = farThreshold.value() / 32767. * 90.;
    pen.setPen(Qt::red);
    pen.setBrush(abs(joyPosition.first) > farThreshold.value() ||
        abs(joyPosition.second) > farThreshold.value() ? Qt::red : Qt::white);
    pen.drawEllipse(QPointF(90, 90), rf, rf);
    pen.setPen(Qt::green);
    pen.setBrush(abs(joyPosition.first) > shortThreshold.value() ||
        abs(joyPosition.second) > shortThreshold.value() ? Qt::green : Qt::white);
    pen.drawEllipse(QPointF(90, 90), rs, rs);

    pen.setPen(Qt::black);
    pen.setBrush(Qt::black);
    pen.drawEllipse(QPointF((joyPosition.first + 32767) / 65536. * 180., (joyPosition.second + 32767) / 65536. * 180.), 4, 4);

    joyMapLabel.setPixmap(QPixmap::fromImage(joyMap));
}

void ThresholdSetter::saveSettings(void)
{
    Joystick::setShortThreshold(shortThreshold.value());
    Joystick::setFarThreshold(farThreshold.value());
    Joystick::saveThresholds(Profile::current());
    close();
}
