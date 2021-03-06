/**
 * @file macrorecorder.h
 * @brief Dialog for recording macro keystrokes.
 */
#ifndef MACRORECORDER_H
#define MACRORECORDER_H

#include <chrono>

#include <QDialog>
#include <QLabel>
#include <QPushButton>

#include "macro.h"

/**
 * @class MacroRecorder
 * @brief Dialog window for recording macros. Keystrokes and timings are
 * recorded.
 */
class MacroRecorder : public QDialog
{
    Q_OBJECT

public:
    explicit MacroRecorder(QWidget *parent = nullptr);

signals:
    /**
     * Emits when recording has finished.
     */
    void recordFinished(ActionList& ks);

public slots:
    /**
     * Called when endRecording is pressed.
     */
    void stopRecording(void);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

    // "Record your macro; press \"STOP\" when finished."
    QLabel lInstructions;
    QPushButton endRecording;

    ActionList keys;
    std::chrono::system_clock::time_point lastTime;
};

#endif // MACRORECORDER_H
