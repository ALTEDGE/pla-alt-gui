#include "colortab.h"
#include "profile.h"
#include "serial.h"

ColorTab::ColorTab(QWidget *parent) :
    QWidget(parent),
    lColorPicker("DRAG CURSOR OVER COLOR", this),
    lColorBrightness("ADJUST BRIGHTNESS WITH BAR", this),
    colorPicker(this),
    colorBrightness(this),
    ledOn("ON", this),
    ledOff("OFF", this),
    outputEnable(true)
{
    // Set control positions
    lColorPicker.setGeometry(0, 10, 640, 20);
    lColorBrightness.setGeometry(0, 280, 640, 20);
    colorPicker.setGeometry(100, 35, 440, 200);
    colorBrightness.setGeometry(150, 250, 340, 30);
    ledOn.setGeometry(20, 115, 60, 40);
    ledOff.setGeometry(560, 115, 60, 40);

    lColorPicker.setAlignment(Qt::AlignCenter);
    lColorBrightness.setAlignment(Qt::AlignCenter);
    colorBrightness.setOrientation(Qt::Horizontal);

    // Brightness range from 0 to 100
    colorBrightness.setRange(0, 50);

    // Connect signals/slots
    connect(&colorPicker, SIGNAL(colorPicked(QColor)), this, SLOT(setColor(QColor)));
    connect(&ledOn, SIGNAL(released()), this, SLOT(enableLeds()));
    connect(&ledOff, SIGNAL(released()), this, SLOT(disableLeds()));
    connect(&colorBrightness, SIGNAL(sliderMoved(int)), this, SLOT(updateBrightness(int)));
}

void ColorTab::showEvent(QShowEvent *event)
{
    // Load settings from config file
    auto& settings = Profile::current();
    currentBrightness = settings.value("color/brightness", 20).toInt();
    auto red = settings.value("color/red", 255).toInt();
    auto green = settings.value("color/green", 0).toInt();
    auto blue = settings.value("color/blue", 0).toInt();

    colorBrightness.setValue(currentBrightness);
    colorPicker.setColor(QColor(red, green, blue));

    event->accept();
}

void ColorTab::setColor(QColor color)
{
    currentColor = color;
    if (outputEnable) {
        serial::sendColor(currentColor.red() * currentBrightness / 100, currentColor.green() *
            currentBrightness / 100, currentColor.blue()  * currentBrightness / 100);
    }

    // Save new color
    auto& settings = Profile::current();
    settings.setValue("color/red", color.red());
    settings.setValue("color/green", color.green());
    settings.setValue("color/blue", color.blue());
}

void ColorTab::enableLeds(void)
{
    outputEnable = true;
    setColor(currentColor);
}

void ColorTab::disableLeds(void)
{
    outputEnable = false;
    // Send "black" to turn off the lights
    serial::sendColor(0, 0, 0);
}

void ColorTab::updateBrightness(int level)
{
    currentBrightness = level;
    if (outputEnable) {
        serial::sendColor(currentColor.red() * currentBrightness / 100, currentColor.green() *
            currentBrightness / 100, currentColor.blue()  * currentBrightness / 100);
    }

    Profile::current().setValue("color/brightness", currentBrightness);
}
