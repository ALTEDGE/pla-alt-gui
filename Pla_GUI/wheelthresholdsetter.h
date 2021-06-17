/**
 * @file wheelthresholdsetter.h
 * @brief Threshold setting dialog for the steering wheel.
 */
#ifndef WHEELTHRESHOLDSETTER_H
#define WHEELTHRESHOLDSETTER_H

#include "input/joysticktracker.h"

#include <QDialog>
#include <QFuture>
#include <QHideEvent>
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
    WheelThresholdSetter(QWidget *parent, QWidget *mainwindow);

private slots:
    /**
     * Saves the threshold values.
     */
    void saveSettings(void);

public:
    /**
     * Starts the primary joystick monitoring thread, to provide a sense of
     * the joystick's range.
     */
    void showEvent(QShowEvent *) override;

    /**
     * Ends the monitoring thread.
     */
    void hideEvent(QHideEvent *event) override;

private:
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
    QFuture<int> joyThread;
};

#endif // WHEELTHRESHOLDSETTER_H
