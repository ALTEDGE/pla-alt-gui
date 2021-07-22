#include "thresholdsetter.h"

#include "controller.h"
#include "joysticktracker.h"
#include "profile.h"

#include <QtConcurrent/QtConcurrent>
#include <QPainter>

#include <atomic>
#include <chrono>
using namespace std::chrono_literals;

constexpr int mapSize = 90;
static std::atomic<JoystickTracker *> currentJoy = nullptr;

ThresholdSetter::ThresholdSetter(QWidget *parent, QWidget *mainwindow) :
    QDialog(parent),
    joyName("PRIMARY"),
    lShortThresh("DEFAULT / VECTOR 1 THRESHOLD", this),
    lFarThresh("VECTOR 2 THRESHOLD", this),
    lCurrentPosition("PRIMARY POSITION", this),
    lInstruction(
        "Passing green threshold\n"
        "triggers action.\n\n"
        "With vector sequencer,\n"
        "passing green triggers Vector 1\n"
        "and passing red triggers Vector 2.", this),
    shortThreshold(this),
    farThreshold(this),
    //currentPrimary(this),
    configSave("SAVE", this),
    configSaveAll("SAVE ALL", this),
    joyMap(mapSize, mapSize, QImage::Format_ARGB32),
    joyMapLabel(this),
    shouldUpdate(false)
{
    setWindowTitle("Trigger Settings");
    setFixedSize(340, 240);
    setModal(true);

    // Set geometries
    lCurrentPosition.setGeometry(20, 10, 180, 20);
    joyMapLabel.setGeometry(20, 30, mapSize, mapSize);
    lInstruction.setGeometry(30 + mapSize, 30, 210, mapSize);
    lShortThresh.setGeometry(20, 130, 300, 20);
    shortThreshold.setGeometry(20, 150, 300, 10);
    lFarThresh.setGeometry(20, 170, 300, 20);
    farThreshold.setGeometry(20, 190, 300, 10);
    configSave.setGeometry(100, 210, 60, 20);
    configSaveAll.setGeometry(180, 210, 60, 20);

    lShortThresh.setAlignment(Qt::AlignCenter);
    lFarThresh.setAlignment(Qt::AlignCenter);
    lInstruction.setAlignment(Qt::AlignLeft | Qt::AlignTop);
    //lCurrentPosition.setAlignment(Qt::AlignCenter);
    shortThreshold.setOrientation(Qt::Horizontal);
    farThreshold.setOrientation(Qt::Horizontal);

    // Set slider ranges
    shortThreshold.setRange(100, 32767);
    farThreshold.setRange(100, 32767);
    shortThreshold.setObjectName("short");
    farThreshold.setObjectName("far");

    connect(&configSave, SIGNAL(released()), this, SLOT(saveSettings()));
    connect(&configSaveAll, SIGNAL(released()), this, SLOT(saveSettingsAll()));
    connect(mainwindow, SIGNAL(exitingProgram()), this, SLOT(close()));
}

void ThresholdSetter::showEvent(QShowEvent *event)
{
    shouldUpdate.store(true);

    auto nameL = joyName.load()[0];
    auto joy = nameL == 'L' ? &Controller::Left
            : (nameL == 'R' ? &Controller::Right
                            : &Controller::Primary);
    shortThreshold.setValue(joy->getShortThreshold());
    farThreshold.setValue(joy->getFarThreshold());
    currentJoy.store(joy);

    // Start the joystick monitoring thread
    joyThread = QThread::create([this] {
        Controller::setEnabled(true);
        Controller::setOperating(false);
        while (shouldUpdate.load()) {
            joyPosition = currentJoy.load()->getPosition();
            updateMap();
            QThread::msleep(50);
        }
        Controller::setOperating(true);
        Controller::setEnabled(false);
    });
    joyThread->start();

    if (event != nullptr)
        event->accept();
}

void ThresholdSetter::hideEvent(QHideEvent *event)
{
    // Bring back the monitor thread
    shouldUpdate.store(false);
    joyThread->wait();
    joyThread->terminate();
    if (event != nullptr)
        event->accept();
}

void ThresholdSetter::setJoystick(QString _name)
{
    if (_name == "LEFT") {
        joyName.store("LEFT");
        currentJoy.store(&Controller::Left);
    } else if (_name == "RIGHT") {
        joyName.store("RIGHT");
        currentJoy.store(&Controller::Right);
    } else {
        joyName.store("PRIMARY");
        currentJoy.store(&Controller::Primary);
    }
    lCurrentPosition.setText(_name + " POSITION");
}

void ThresholdSetter::updateMap(void)
{
    QPainter pen (&joyMap);
    QPen linePen;
    long double dist = sqrt(static_cast<long double>(joyPosition.first * joyPosition.first) +
        static_cast<long double>(joyPosition.second * joyPosition.second));
    auto rf = farThreshold.value() / 32767. * static_cast<double>(mapSize / 2.);
    auto rs = shortThreshold.value() / 32767. * static_cast<double>(mapSize / 2.);

    pen.fillRect(0, 0, mapSize, mapSize, dist > farThreshold.value() ? Qt::red : Qt::black);
    linePen.setWidth(2);
    linePen.setColor(Qt::white);
    pen.setPen(linePen);
    pen.drawRect(0, 0, mapSize, mapSize);

    linePen.setColor(Qt::red);
    pen.setBrush(dist > shortThreshold.value() && dist < farThreshold.value() ?
        Qt::green : Qt::black);
    pen.setPen(linePen);
    pen.drawEllipse(QPointF(mapSize / 2, mapSize / 2), rf, rf);

    linePen.setColor(Qt::green);
    pen.setBrush(Qt::black);
    pen.setPen(linePen);
    pen.drawEllipse(QPointF(mapSize / 2, mapSize / 2), rs, rs);

    pen.setPen(Qt::gray);
    pen.setBrush(Qt::gray);
    pen.drawLine(mapSize / 4, 0, mapSize * 3 / 4, mapSize);
    pen.drawLine(mapSize * 3 / 4, 0, mapSize / 4, mapSize);
    pen.drawLine(0, mapSize / 4, mapSize, mapSize * 3 / 4);
    pen.drawLine(0, mapSize * 3 / 4, mapSize, mapSize / 4);

    pen.setPen(Qt::white);
    pen.setBrush(Qt::white);
    pen.drawEllipse(QPointF((joyPosition.first + 32767) / 65536. * static_cast<double>(mapSize),
        mapSize - ((joyPosition.second + 32767) / 65536. * static_cast<double>(mapSize))), 3, 3);

    joyMapLabel.setPixmap(QPixmap::fromImage(joyMap));
}

void ThresholdSetter::saveSettings(void)
{
    // Set sliders to current settings
    auto nameL = joyName.load()[0];
    if (nameL == 'L') {
        Controller::Left.setShortThreshold(shortThreshold.value());
        Controller::Left.setFarThreshold(farThreshold.value());
    } else if (nameL == 'R') {
        Controller::Right.setShortThreshold(shortThreshold.value());
        Controller::Right.setFarThreshold(farThreshold.value());
    } else if (nameL == 'P') {
        Controller::Primary.setShortThreshold(shortThreshold.value());
        Controller::Primary.setFarThreshold(farThreshold.value());
    }
    Controller::save(Profile::current());
    Profile::save();

    close();
}

void ThresholdSetter::saveSettingsAll(void)
{
    int s = shortThreshold.value(), f = farThreshold.value();
    Controller::Left.setShortThreshold(s);
    Controller::Right.setShortThreshold(s);
    Controller::Primary.setShortThreshold(s);
    Controller::Left.setFarThreshold(f);
    Controller::Right.setFarThreshold(f);
    Controller::Primary.setFarThreshold(f);
    Controller::save(Profile::current());
    Profile::save();

    close();
}
