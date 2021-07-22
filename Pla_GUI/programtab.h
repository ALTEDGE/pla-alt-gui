/**
 * @file programtab.h
 * @brief Code for the Programming tab.
 */
#ifndef PROGRAMTAB_H
#define PROGRAMTAB_H

#include <QButtonGroup>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QShowEvent>
#include <QWidget>

#include "editing.h"
#include "joysticktracker.h"
#include "controller.h"
#include "keygrabber.h"
#include "savabletab.h"
#include "thresholdsetter.h"

/**
 * @class ProgramTab
 * @brief Provides controls to program the controller.
 */
class ProgramTab : public SavableTab
{
    Q_OBJECT

public:
    explicit ProgramTab(QWidget *parent = nullptr);

private slots:
    /**
     * Updates labels and key slots for the selected joystick.
     */
    void updateControls(void);

    /**
     * Enables or disables diagonals for the currently selected joystick,
     * showing/hiding controls as necessary.
     * @param enabled True to enable diagonal movement
     */
    void setDiagonals(bool enabled);

    /**
     * Enables or disables vector sequencing, showing/hiding 2nd vector
     * controls as necessary.
     * @param enabled True to enable sequencing
     */
    void setSequencer(bool enabled);

    void setButtonSticky(bool enabled);

    /**
     * Assigns the given key slot for the currently selected joystick.
     * @param index Index of the key slot
     */
    void assignSlot(int index);
    void assignButton(void);

    /**
     * Captures key presses from the KeyGrabber dialog.
     */
    void keyPressed(Key key);

    /**
     * Opens the threshold settings window.
     */
    void openThresholdSettings(void);

    /**
     * Saves all controller settings.
     */
    void saveSettings(void);

    /**
     * Loads controller settings (reverting changes if any were made).
     */
    void loadSettings(void);

    /**
     * From SavableTab, tests to see if any settings have changed.
     */
    bool isModified(void) const;

private:
    void showEvent(QShowEvent *event);

    QLabel lSelectJoystick;
    QLabel lEnterKeyOrMacro;
    QLabel lVector1;
    QLabel lVector2;
    QLabel lJoystick;
    QLabel lSequencer;
    QLabel lButtonAction;
    QLabel separator;

    QLabel lJoystickGuide;
    QLabel lPGJoystick[8];
    QPixmap pixJoystick;
    QPixmap pixPJoystick;

    QLabel lArrow[3];
    QPixmap pixArrow;
    QLabel lArrowDown;
    QPixmap pixArrowDown;

    QRadioButton useLeftJoystick;
    QRadioButton useRightJoystick;
    QRadioButton usePrimaryJoystick;

    QButtonGroup pgButtons;
    QPushButton joystickButton;
    QButtonGroup keySlots;
    QLabel lKeySlots[8];
    QLabel lKeySlots2[8];

    QCheckBox useDiagonals;
    QCheckBox useSequencer;
    QCheckBox useLatch;

    QPushButton configSave;
    QPushButton configCancel;
    QPushButton configThreshold;

    Editing<JoystickTracker> leftData;
    Editing<JoystickTracker> rightData;
    Editing<PrimaryJoystickTracker> primaryData;

    KeyGrabber keyAssignDialog;
    ThresholdSetter thresholdDialog;
    int assigningSlot;
};

#endif // PROGRAMTAB_H
