/**
 * @file colortab.h
 * @brief Code for the Color tab.
 */
#ifndef COLORTAB_H
#define COLORTAB_H

#include <QWidget>
#include <QLabel>
#include <QShowEvent>
#include <QSlider>
#include <QTimer>
#include <QPushButton>
#include <wwWidgets/QwwHueSatPicker>

/**
 * @class ColorTab
 * @brief Provides controls for setting the controller's LED color.
 */
class ColorTab: public QWidget
{
    Q_OBJECT

public:
    explicit ColorTab(QWidget *parent = nullptr);

private slots:
    /**
     * Sets the current color, and sends it to the controller if output is
     * enabled.
     * @param color The selected color
     */
    void setColor(QColor color);

    /**
     * Turns on the controller's lights.
     */
    void enableLeds(void);

    /**
     * Turns off the controller's lights.
     */
    void disableLeds(void);

    /**
     * Updates the color's brightness, and sends it to the controller if
     * output is enabled.
     * @param level Brightness from 0-100
     */
    void updateBrightness(int level);

    /**
     * Sends the latest color setting to the controller.
     */
    void updateColor(void);

private:
    void showEvent(QShowEvent *event);

    // "DRAG CURSOR OVER COLOR"
    QLabel lColorPicker;
    // "ADJUST BRIGHTNESS WITH BAR"
    QLabel lColorBrightness;

    QLabel lLedOn;
    QLabel lLedOff;

    QwwHueSatPicker colorPicker;
    QSlider colorBrightness;

    QPushButton ledOn;
    QPushButton ledOff;

    QTimer updateTimer;
};

#endif // COLORTAB_H
