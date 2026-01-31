#include "colortab.h"
#include "profile.h"
#include "serial.h"

#include <QStyleOptionFrame>
#include <QThread>

ColorTab::ColorTab(QWidget *parent) :
    QWidget(parent),
    lColorPicker("DRAG CURSOR OVER COLOR", this),
    lColorBrightness("ADJUST BRIGHTNESS WITH BAR", this),
    lLedOn("ON", this),
    lLedOff("OFF", this),
    lLedsCase("CASE - LEDS", this),
    lLedsJoy("JOY - LEDS", this),
    colorPicker(this),
    colorBrightness(this),
    ledOn(this),
    ledOff(this),
    ledsCase(this),
    ledsJoy(this),
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
    lLedsCase.setGeometry(68, 360, 75, 20);
    lLedsJoy.setGeometry(768, 360, 75, 20);
    colorPicker.setGeometry(200, 45, 500, 300);
    colorBrightness.setGeometry(275, 360, 337, 74);
    ledOn.setGeometry(68, 160, 75, 79);
    ledOff.setGeometry(768, 160, 75, 79);
    ledsCase.setGeometry(89, 385, 50, 30);
    ledsJoy.setGeometry(786, 385, 50, 30);

    lColorPicker.setAlignment(Qt::AlignCenter);
    lColorBrightness.setAlignment(Qt::AlignCenter);
    lLedOn.setAlignment(Qt::AlignCenter);
    lLedOff.setAlignment(Qt::AlignCenter);
    lLedsCase.setAlignment(Qt::AlignCenter);
    lLedsJoy.setAlignment(Qt::AlignCenter);
    colorBrightness.setOrientation(Qt::Horizontal);

    // Limit brightness range - high brightness looks bad
    colorBrightness.setRange(0, 100);

    colorPicker.setCursor(Qt::CrossCursor);
    updateTimer.setSingleShot(true);

    // Connect signals/slots
    connect(&colorPicker, SIGNAL(colorPicked(QColor)), this, SLOT(setColor(QColor)));
    connect(&ledOn, SIGNAL(released()), this, SLOT(enableLeds()));
    connect(&ledOff, SIGNAL(released()), this, SLOT(disableLeds()));
    connect(&ledsCase, SIGNAL(toggled(bool)), this, SLOT(caseChecked(bool)));
    connect(&ledsJoy, SIGNAL(toggled(bool)), this, SLOT(joyChecked(bool)));
    connect(&colorBrightness, SIGNAL(sliderMoved(int)), this, SLOT(updateBrightness(int)));
    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(updateColor()));
}

void ColorTab::updateColor(void)
{
    if (ledsCase.isChecked()) {
        Controller::CaseColor = led;
        Controller::updateColor();
    } else if (ledsJoy.isChecked()) {
        Controller::JoyColor = led;
        Controller::updateColor();
    }
}

void ColorTab::caseChecked(bool check)
{
    if (check) {
        led = Controller::CaseColor;
        colorBrightness.setValue(led.brightness);
        colorPicker.setColor(led.color);
    }
}

void ColorTab::joyChecked(bool check)
{
    if (check) {
        led = Controller::JoyColor;
        colorBrightness.setValue(led.brightness);
        colorPicker.setColor(led.color);
    }
}

void ColorTab::showEvent(QShowEvent *event)
{
    ledsJoy.setChecked(true);
    joyChecked(true);

    event->accept();
}

void ColorTab::setColor(QColor color)
{
    led.color = color;
    if (!updateTimer.isActive())
        updateTimer.start(100);
}

void ColorTab::enableLeds(void)
{
    led.enabled = true;
    if (!updateTimer.isActive())
        updateTimer.start(100);
}

void ColorTab::disableLeds(void)
{
    led.enabled = false;
    if (!updateTimer.isActive())
        updateTimer.start(100);
}

void ColorTab::updateBrightness(int level)
{
    led.brightness = level;
    if (!updateTimer.isActive())
        updateTimer.start(100);
}
