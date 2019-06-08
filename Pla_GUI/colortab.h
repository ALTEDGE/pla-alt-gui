#ifndef COLORTAB_H
#define COLORTAB_H

#include <QWidget>
#include <QLabel>
#include <QShowEvent>
#include <QSlider>
#include <QPushButton>
#include <wwWidgets/QwwHueSatPicker>

/**
 * @class ColorTab
 * @brief Provides controls for setting the controller's color.
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

private:
    void showEvent(QShowEvent *event);

    // "DRAG CURSOR OVER COLOR"
    QLabel lColorPicker;
    // "ADJUST BRIGHTNESS WITH BAR"
    QLabel lColorBrightness;

    QwwHueSatPicker colorPicker;
    QSlider colorBrightness;

    QPushButton ledOn;
    QPushButton ledOff;

    /**
     * If true, setting changes will be immediately sent to the controller.
     */
    bool outputEnable;

    int currentBrightness;
    QColor currentColor;
};

#endif // COLORTAB_H
