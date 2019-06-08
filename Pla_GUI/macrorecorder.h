#ifndef MACRORECORDER_H
#define MACRORECORDER_H

#include <chrono>

#include <QDialog>
#include <QLabel>
#include <QPushButton>

#include "macro.h"

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
    bool eventFilter(QObject *watched, QEvent *event);

private:
    void showEvent(QShowEvent *event);

    // "Record your macro; press \"STOP\" when finished."
    QLabel lInstructions;
    QPushButton endRecording;

    ActionList keys;
    std::chrono::system_clock::time_point lastTime;
};

#endif // MACRORECORDER_H
