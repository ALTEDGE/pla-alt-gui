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

    /**
     * Assigns the given key slot for the currently selected joystick.
     * @param index Index of the key slot
     */
    void assignSlot(int index);

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

    // "SELECT JOYSTICK"
    QLabel lSelectJoystick;
    // e.g. "MAP_PG4", or "L. AUX"
    QLabel lSelectedPG;
    // "ENTER KEY OR MACRO COMMAND"
    QLabel lEnterKeyOrMacro;
    // "VECTOR 1 COMMAND"
    QLabel lVector1;
    // "VECTOR 2 COMMAND"
    QLabel lVector2;

    QLabel lJoystickGuide;

    QRadioButton useLeftJoystick;
    QRadioButton useRightJoystick;
    QRadioButton usePrimaryJoystick;

    QButtonGroup pgButtons;
    QButtonGroup keySlots;
    QLabel *lKeySlots[8];

    QCheckBox useDiagonals;
    QCheckBox useSequencer;

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
