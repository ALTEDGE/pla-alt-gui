#include "colortab.h"
#include "controller.h"
#include "profile.h"
#include "serial.h"

#include <QThread>

ColorTab::ColorTab(QWidget *parent) :
    QWidget(parent),
    lColorPicker("DRAG CURSOR OVER COLOR", this),
    lColorBrightness("ADJUST BRIGHTNESS WITH BAR", this),
    lLedOn("ON", this),
    lLedOff("OFF", this),
    colorPicker(this),
    colorBrightness(this),
    ledOn(this),
    ledOff(this),
    updateTimer(this)
{
    ledOn.setIcon(QIcon("assets/color-on.png"));
    ledOn.setIconSize(QSize(75, 79));
    ledOff.setIcon(QIcon("assets/color-off.png"));
    ledOff.setIconSize(QSize(75, 79));

    // Set control positions
    lColorPicker.setGeometry(0, 20, 900, 20);
    lColorBrightness.setGeometry(0, 444, 900, 20);
    lLedOn.setGeometry(68, 160 + 79, 75, 20);
    lLedOff.setGeometry(768, 160 + 79, 75, 20);
    colorPicker.setGeometry(200, 45, 500, 300);
    colorBrightness.setGeometry(275, 360, 337, 74);
    ledOn.setGeometry(68, 160, 75, 79);
    ledOff.setGeometry(768, 160, 75, 79);

    lColorPicker.setAlignment(Qt::AlignCenter);
    lColorBrightness.setAlignment(Qt::AlignCenter);
    lLedOn.setAlignment(Qt::AlignCenter);
    lLedOff.setAlignment(Qt::AlignCenter);
    colorBrightness.setOrientation(Qt::Horizontal);

    // Brightness range from 0 to 50 (high brightness looks bad)
    colorBrightness.setRange(0, 50);

    colorPicker.setCursor(Qt::CrossCursor);
    updateTimer.setSingleShot(true);

    // Connect signals/slots
    connect(&colorPicker, SIGNAL(colorPicked(QColor)), this, SLOT(setColor(QColor)));
    connect(&ledOn, SIGNAL(released()), this, SLOT(enableLeds()));
    connect(&ledOff, SIGNAL(released()), this, SLOT(disableLeds()));
    connect(&colorBrightness, SIGNAL(sliderMoved(int)), this, SLOT(updateBrightness(int)));
    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(updateColor()));
}

void ColorTab::updateColor(void)
{
    Controller::updateColor();
}

void ColorTab::showEvent(QShowEvent *event)
{
    colorBrightness.setValue(Controller::ColorBrightness);
    colorPicker.setColor(Controller::Color);

    event->accept();
}

void ColorTab::setColor(QColor color)
{
    Controller::Color = color;
    if (!updateTimer.isActive())
        updateTimer.start(100);
}

void ColorTab::enableLeds(void)
{
    Controller::ColorEnable = true;
    if (!updateTimer.isActive())
        updateTimer.start(100);
}

void ColorTab::disableLeds(void)
{
    Controller::ColorEnable = false;
    if (!updateTimer.isActive())
        updateTimer.start(100);
}

void ColorTab::updateBrightness(int level)
{
    Controller::ColorBrightness = level;
    if (!updateTimer.isActive())
        updateTimer.start(100);
}
