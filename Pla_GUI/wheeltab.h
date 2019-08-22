/**
 * @file wheeltab.h
 * @brief Code for the Wheel tab.
 */
#ifndef WHEELTAB_H
#define WHEELTAB_H

#include "editing.h"
#include "keygrabber.h"
#include "savabletab.h"
#include "steeringtracker.h"
#include "wheelthresholdsetter.h"

#include <QPushButton>
#include <QRadioButton>
#include <QShowEvent>
#include <QWidget>

/**
 * @class WheelTab
 * @brief Provides controls to modify settings for the steering wheel
 */
class WheelTab : public SavableTab
{
    Q_OBJECT

public:
    explicit WheelTab(QWidget *parent = nullptr);

private slots:
    /**
     * Prompts the user to assign the left steer's key.
     */
    void assignLeft(void);

    /**
     * Prompt the user to assign the right steer's key.
     */
    void assignRight(void);

    void openThresholdSettings(void);

    /**
     * Catches key press data from the KeyGrabber dialog.
     */
    void keyPressed(Key key);

    /**
     * Catches change in digital/analog selection.
     */
    void setWheelFunction(bool digital);

    /**
     * Save all settings.
     */
    void saveSettings(void);

    /**
     * Loads all settings, discarding unsaved changes.
     */
    void loadSettings(void);

    /**
     * Returns true if unsaved changes have been made.
     */
    bool isModified(void) const;

private:
    void showEvent(QShowEvent *event);
    void paintEvent(QPaintEvent *event);

    QLabel lLeftAction;
    QLabel lRightAction;

    QRadioButton steerDigital;
    QRadioButton steerAnalog;

    KeyGrabber keyGrabber;
    QPushButton leftAction;
    QPushButton rightAction;

    QPushButton configSave;
    QPushButton configCancel;
    QPushButton configThreshold;

    Editing<SteeringTracker> steerData;
    WheelThresholdSetter thresholdDialog;

    int activeAction;
};

#endif // WHEELTAB_H
