#ifndef PROGRAMMINGTAB_H
#define PROGRAMMINGTAB_H

#include "axisselector.h"
#include "keygrabber.h"

#include <QWidget>

/**
 * @class ProgrammingTab
 * @brief Provides controls to set actions for joysticks/PGs.
 */
class ProgrammingTab : public QWidget
{
    Q_OBJECT

public:
    explicit ProgrammingTab(QWidget *parent = nullptr);

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
    void keyPressed(Key key);

    void diagonalChecked(bool state);

private:
    /**
     * Updates window controls for the given PG number.
     */
    void loadPG(int n);

    AxisSelector axisSel;

    QPushButton *actions[8];
    QPushButton *lastPressed;
    KeyGrabber keyDialog;

    // e.g. "MAP_PG1", or "R. AUX"
    QLabel selectedPG;
    // "ENTER KEY OR MACRO COMMANDS"
    QLabel instruction;

    int currentPG;
};

#endif // PROGRAMMINGTAB_H
