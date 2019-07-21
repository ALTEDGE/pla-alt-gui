#include "thresholdsetter.h"

#include "controller.h"
#include "joysticktracker.h"
#include "profile.h"

ThresholdSetter::ThresholdSetter(QWidget *parent) :
    QDialog(parent),
    lShortThresh("DEFAULT / VECTOR 1 THRESHOLD", this),
    lFarThresh("VECTOR 2 THRESHOLD", this),
    lCurrentPrimary("PRIMARY X+ CURRENT POSITION", this),
    shortThreshold(this),
    farThreshold(this),
    currentPrimary(this),
    configSave("SAVE", this),
    shouldUpdate(true)
{
    setWindowTitle("Threshold Settings");
    setFixedSize(240, 160);

    // Set geometries
    lShortThresh.setGeometry(0, 10, 240, 20);
    lFarThresh.setGeometry(0, 50, 240, 20);
    lCurrentPrimary.setGeometry(0, 90, 240, 20);
    shortThreshold.setGeometry(30, 30, 180, 10);
    farThreshold.setGeometry(30, 70, 180, 10);
    currentPrimary.setGeometry(30, 110, 180, 10);
    configSave.setGeometry(90, 130, 60, 20);

    lShortThresh.setAlignment(Qt::AlignCenter);
    lFarThresh.setAlignment(Qt::AlignCenter);
    lCurrentPrimary.setAlignment(Qt::AlignCenter);
    shortThreshold.setOrientation(Qt::Horizontal);
    farThreshold.setOrientation(Qt::Horizontal);
    currentPrimary.setOrientation(Qt::Horizontal);

    // Set slider ranges
    shortThreshold.setRange(100, 32767);
    farThreshold.setRange(100, 32767);
    currentPrimary.setRange(100, 32767);

    connect(&configSave, SIGNAL(released()), this, SLOT(saveSettings()));
}

void ThresholdSetter::showEvent(QShowEvent *event)
{
    shouldUpdate = true;

    // Set sliders to current settings
    shortThreshold.setValue(Joystick::getShortThreshold());
    farThreshold.setValue(Joystick::getFarThreshold());

    // Start the joystick monitoring thread
    updateCurrent = std::thread([&](void) {
        while (shouldUpdate) {
            currentPrimary.setValue(Controller::Primary.getLastX());
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
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

void ThresholdSetter::saveSettings(void)
{
    Joystick::setShortThreshold(shortThreshold.value());
    Joystick::setFarThreshold(farThreshold.value());
    Joystick::saveThresholds(Profile::current());
    close();
}
