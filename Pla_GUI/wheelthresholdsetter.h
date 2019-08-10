#ifndef WHEELTHRESHOLDSETTER_H
#define WHEELTHRESHOLDSETTER_H

#include "input/joysticktracker.h"

#include <QCloseEvent>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QShowEvent>
#include <QSlider>

#include <thread>

/**
 * @class ThresholdSetter
 * @brief Provides controls to set the joystick's action thresholds.
 */
class WheelThresholdSetter : public QDialog
{
    Q_OBJECT

public:
    WheelThresholdSetter(QWidget *parent = nullptr);

private slots:
    /**
     * Saves the threshold values.
     */
    void saveSettings(void);

private:
    /**
     * Starts the primary joystick monitoring thread, to provide a sense of
     * the joystick's range.
     */
    void showEvent(QShowEvent *);

    /**
     * Ends the monitoring thread.
     */
    void closeEvent(QCloseEvent *event);

    void updateMap(void);

    // "WHEEL POSITION"
    QLabel lMap;
    // "WHEEL THRESHOLD"
    QLabel lThresh;

    QSlider threshold;

    QPushButton configSave;
    QImage map;
    QLabel mapLabel;

    std::thread updateCurrent;
    int position;
    bool shouldUpdate;
};

#endif // WHEELTHRESHOLDSETTER_H
