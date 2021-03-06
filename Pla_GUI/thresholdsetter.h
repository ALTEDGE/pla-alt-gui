/**
 * @file thresholdsetter.h
 * @brief Dialog for managing joystick thresholds.
 */
#ifndef THRESHOLDSETTER_H
#define THRESHOLDSETTER_H

#include "input/joysticktracker.h"

#include <QDialog>
#include <QHideEvent>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QShowEvent>
#include <QSlider>
#include <QThread>

#include <atomic>
#include <thread>

/**
 * @class ThresholdSetter
 * @brief Provides controls to set the joystick's action thresholds.
 */
class ThresholdSetter : public QDialog
{
    Q_OBJECT

public:
    ThresholdSetter(QWidget *parent, QWidget *mainwindow);

    /**
     * Sets the joystick to be managed.
     * Options are: "LEFT", "RIGHT", and "PRIMARY". Anything else is undefined
     * behavior.
     */
    void setJoystick(QString _name);

    bool eventFilter(QObject *object, QEvent *event) override;

private slots:
    /**
     * Saves the threshold values.
     */
    void saveSettings(void);

    /**
     * Saves the threshold values for all joysticks.
     * Includes left, primany, and right (not steering).
     */
    void saveSettingsAll(void);

    void onPrimaryWidthChanged(int);
    void onThresholdsChanged(int);

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
    /**
     * Updates the joystick position map.
     */
    void updateMap(void);

    std::atomic<const char *> joyName;

    QLabel lShortThresh;
    QLabel lFarThresh;
    QLabel lPrimaryWidth;
    QLabel lCurrentPosition;
    QLabel lInstruction;

    QSlider shortThreshold;
    QSlider farThreshold;
    QSlider primaryWidth;

    QPushButton configSave;
    QPushButton configSaveAll;
    QImage joyMap;
    QLabel joyMapLabel;

    std::thread updateCurrent;
    std::pair<int, int> joyPosition;
    std::atomic_bool shouldUpdate;
    QThread *joyThread;
    std::pair<int, int> mapLineDivs;
};

#endif // THRESHOLDSETTER_H
