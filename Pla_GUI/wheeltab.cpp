#include "wheeltab.h"
#include "controller.h"
#include "profile.h"

WheelTab::WheelTab(QWidget *parent) :
    SavableTab(parent),
    steerDigital("DIGITAL STEERING", this),
    steerAnalog("ANALOG STEERING", this),
    keyGrabber(this),
    leftAction("", this),
    rightAction("", this),
    configSave("SAVE", this),
    configCancel("CANCEL", this),
    configThreshold("THRESHOLD SETTINGS", this),
    steerData(Controller::Steering),
    thresholdDialog(this)
{
    // Set geometries
    steerDigital.setGeometry(60, 60, 140, 20);
    steerDigital.setLayoutDirection(Qt::RightToLeft);
    steerAnalog.setGeometry(60, 100, 140, 20);
    steerAnalog.setLayoutDirection(Qt::RightToLeft);
    leftAction.setGeometry(300, 60, 140, 20);
    rightAction.setGeometry(450, 60, 140, 20);
    configSave.setGeometry(235, 320, 80, 20);
    configCancel.setGeometry(325, 320, 80, 20);
    configThreshold.setGeometry(30, 320, 130, 20);

    // Connect signals/slots
    connect(&leftAction, SIGNAL(released()), this, SLOT(assignLeft()));
    connect(&rightAction, SIGNAL(released()), this, SLOT(assignRight()));
    connect(&keyGrabber, SIGNAL(keyPressed(Key)), this, SLOT(keyPressed(Key)));
    connect(&steerDigital, SIGNAL(toggled(bool)), this, SLOT(setWheelFunction(bool)));
    connect(&configSave, SIGNAL(released()), this, SLOT(saveSettings()));
    connect(&configCancel, SIGNAL(released()), this, SLOT(loadSettings()));
    connect(&configThreshold, SIGNAL(released()), this, SLOT(openThresholdSettings()));

    loadSettings();
}

void WheelTab::showEvent(QShowEvent *event)
{
    // Get Editing objects up-to-date
    steerData.save();

    // Set key slots
    leftAction.setText(steerData->getText(0));
    rightAction.setText(steerData->getText(1));
    leftAction.setToolTip(leftAction.text());
    rightAction.setToolTip(rightAction.text());

    // Set digital/analog
    bool digital = steerData->getDigital();
    (digital ? steerDigital : steerAnalog).setChecked(true);
    setWheelFunction(digital);

    if (event != nullptr)
        event->accept();
}

bool WheelTab::isModified(void) const
{
    return steerData.isModified();
}

void WheelTab::loadSettings(void)
{
    if (steerData.isModified())
        steerData.revert();

    showEvent(nullptr);
}

void WheelTab::saveSettings(void)
{
    steerData.save();
    Controller::save(Profile::current());
}

void WheelTab::setWheelFunction(bool digital)
{
    // Make buttons visible if in digital mode
    leftAction.setVisible(digital);
    rightAction.setVisible(digital);

    steerData->setDigital(digital);
}

void WheelTab::assignLeft(void)
{
    // activeAction tracks whether left or right is being assigned
    activeAction = 0;
    keyGrabber.show();
}

void WheelTab::assignRight(void)
{
    activeAction = 1;
    keyGrabber.show();
}

void WheelTab::openThresholdSettings(void)
{
    thresholdDialog.show();
}

void WheelTab::keyPressed(Key key)
{
    // activeAction: 0 = left, 1 = right
    steerData->setKey(activeAction, key);

    // Update text
    auto& slot = activeAction == 0 ? leftAction : rightAction;
    auto text = key.toString();
    slot.setText(text);
    slot.setToolTip(text);
}
