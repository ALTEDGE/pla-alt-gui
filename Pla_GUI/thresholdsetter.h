#ifndef THRESHOLDSETTER_H
#define THRESHOLDSETTER_H

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
class ThresholdSetter : public QDialog
{
    Q_OBJECT

public:
    ThresholdSetter(QWidget *parent = nullptr);

    void setJoystick(QString _name);

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

    QString name;

    // "DEFAULT / VECTOR 1 THRESHOLD"
    QLabel lShortThresh;
    // "VECTOR 2 THRESHOLD"
    QLabel lFarThresh;
    // "CURRENT POSITION"
    QLabel lCurrentPosition;

    QSlider shortThreshold;
    QSlider farThreshold;

    QPushButton configSave;
    QImage joyMap;
    QLabel joyMapLabel;

    std::thread updateCurrent;
    std::pair<int, int> joyPosition;
    bool shouldUpdate;
};

#endif // THRESHOLDSETTER_H
