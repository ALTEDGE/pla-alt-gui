#ifndef THRESHOLDSETTER_H
#define THRESHOLDSETTER_H

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

    // "DEFAULT / VECTOR 1 THRESHOLD"
    QLabel lShortThresh;
    // "VECTOR 2 THRESHOLD"
    QLabel lFarThresh;
    // "PRIMARY X+ CURRENT POSITION"
    QLabel lCurrentPrimary;

    QSlider shortThreshold;
    QSlider farThreshold;
    QSlider currentPrimary;

    QPushButton configSave;

    std::thread updateCurrent;
    bool shouldUpdate;
};

#endif // THRESHOLDSETTER_H
