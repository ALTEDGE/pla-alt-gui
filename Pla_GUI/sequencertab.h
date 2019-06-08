#ifndef SEQUENCERTAB_H
#define SEQUENCERTAB_H

#include "axisselector.h"
#include "keygrabber.h"

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>

/**
 * @class SequencerTab
 * @brief Provides controls to set keys for vector sequencing
  */
class SequencerTab : public QWidget
{
    Q_OBJECT

public:
    explicit SequencerTab(QWidget *parent = nullptr);

private slots:
    /**
     * Catches axisSel's PG change signal to update the action assign buttons.
     */
    void setPG(int);

    /**
     * Catches axisSel's selected joystick change to update the action assign buttons.
     */
    void joystickChanged(void);

    /**
     * Catches releases of the assignment buttons, to open the KeyGrabber prompt.
     */
    void setAction(void);

    /**
     * Catches the keypress from the KeyGrabber prompt.
     */
    void keyPressed(Key);

    /**
     * Handles vector sequencer enabling/disabling.
     */
    void sequencerToggled(bool);

private:
    /**
     * Updates the action assigning buttons.
     */
    void updateActionButtons(int n);

    int currentPG;

    AxisSelector axisSel;
    KeyGrabber keyDialog;
    QLabel selectedActions;

    // "ENTER KEY OR MACRO COMMANDS"
    QLabel instruction;
    // "VECTOR 1 COMMAND"
    QLabel vector1;
    // "VECTOR 2 COMMAND"
    QLabel vector2;

    QCheckBox enableSequencer;
    QPushButton *activeAction;
    QPushButton *actions[16];
};

#endif // SEQUENCERTAB_H
