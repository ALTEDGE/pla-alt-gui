#include "wheeltab.h"
#include "controller.h"
#include "profile.h"

#include <QPainter>

WheelTab::WheelTab(QWidget *parent) :
    SavableTab(parent),
    lLeftAction("WHEEL LEFT", this),
    lRightAction("WHEEL RIGHT", this),
    steerDigital("DIGITAL STEERING", this),
    steerAnalog("ANALOG STEERING", this),
    keyGrabber(this),
    leftAction("", this),
    rightAction("", this),
    configSave("SAVE", this),
    configCancel("CANCEL", this),
    configThreshold("TRIGGER SETTINGS", this),
    steerData(Controller::Steering),
    thresholdDialog(this, parent)
{
    // Set geometries
    lLeftAction.setGeometry(70, 40, 90, 20);
    lRightAction.setGeometry(190, 40, 90, 20);
    leftAction.setGeometry(70, 60, 90, 30);
    rightAction.setGeometry(190, 60, 90, 30);
    steerDigital.setGeometry(70, 105, 210, 30);
    steerAnalog.setGeometry(70, 150, 210, 30);
    configThreshold.setGeometry(70, 190, 170, 20);
    configSave.setGeometry(70, 220, 80, 20);
    configCancel.setGeometry(160, 220, 80, 20);

    lLeftAction.setAlignment(Qt::AlignCenter);
    lRightAction.setAlignment(Qt::AlignCenter);
    //steerDigital.setLayoutDirection(Qt::RightToLeft);
    //steerAnalog.setLayoutDirection(Qt::RightToLeft);

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

void WheelTab::paintEvent(QPaintEvent *event)
{
    static QImage background ("assets/wheelbg.png");
    QPainter paint (this);
    paint.drawImage(0, 10, background);

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
    lLeftAction.setVisible(digital);
    lRightAction.setVisible(digital);
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
