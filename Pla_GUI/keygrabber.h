/**
 * @file keygrabber.h
 * @brief Provides dialog for capturing the keystroke for a binding.
 */
#ifndef KEYGRABBER_H
#define KEYGRABBER_H

#include "key.h"

#include <QComboBox>
#include <QDialog>
#include <QEvent>
#include <QLabel>
#include <QPushButton>

/**
 * @class KeyGrabber
 * @brief Prompts the user for a keystroke, recording the input.
 * Additionally, a macro may be selected instead; or, no keystroke may be
 * reported.
 */
class KeyGrabber : public QDialog
{
    Q_OBJECT

signals:
    /**
     * Emitted when a valid keypress is received.
     */
    void keyPressed(Key);

private slots:
    /**
     * Catches useNone's release, to have keyPressed() send an empty QKeyEvent.
     */
    void noneClicked(void);

    /**
     * Catches useMacro's release, to use the selected macro.
     */
    void macroClicked(void);

public:
    KeyGrabber(QWidget *parent = nullptr);

protected:
    /**
     * Filters through events for key releases.
     * Upon capture, fires the keyPressed signal with the key data.
     * @see keyPressed()
     */
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    /**
     * Reloads the macro list on every call to show().
     */
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

    // "Press a key (or combination), or select a macro."
    QLabel lInstructions;

    QPushButton useNone;
    QPushButton useMacro;

    QComboBox macroNames;
};

#endif // KEYGRABBER_H
