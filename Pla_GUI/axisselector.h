#ifndef AXISSELECTOR_H
#define AXISSELECTOR_H

#include <QWidget>
#include <QLabel>
#include <QRadioButton>
#include <QButtonGroup>
#include <QCheckBox>
#include <QPushButton>

/**
 * @class AxisSelector
 * @brief Provides the common axis and PG group selection controls.
 * This is shared between ProgrammingTab and the vector sequencer.
 */
class AxisSelector : public QWidget
{
    Q_OBJECT

public:
    explicit AxisSelector(QWidget *parent = nullptr);

    /**
     * Checks if the left auxilary joystick is currently selected.
     * @return true if selected
     */
    bool left(void) const;

    /**
     * Checks if the primary joystick is currently selected.
     * @return true if selected
     */
    bool primary(void) const;

    /**
     * Checks if the right auxilary joystick is currently selected.
     * @return true if selected
     */
    bool right(void) const;

signals:
    /**
     * Signaled when a PG button is pressed.
     * @param pg number of the selected PG (zero-based, 0-7)
     */
    void released(int pg);

    /**
     * Signaled when the selected joystick changes.
     * The new selection can be checked for with left(), primary(), and right().
     */
    void joystickChanged(void);

    void diagonalChecked(bool enable);

private slots:
    /**
     * Used to capture button releases for the PG button group.
     */
    void onRelease(int b);

    /**
     * Used to capture joystick changes.
     */
    void onJoystickChange(void);

    void onDiagonalClick(bool);

private:
    // "SELECT JOYSTICK"
    QLabel selectLabel;

    QRadioButton selectPrimary;
    QRadioButton selectLeft;
    QRadioButton selectRight;
    QCheckBox enableDiagonal;

    QButtonGroup mapPGGroup;
};

#endif // AXISSELECTOR_H
