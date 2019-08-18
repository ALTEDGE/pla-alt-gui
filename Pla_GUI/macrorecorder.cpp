#include "macrorecorder.h"

#include <QKeyEvent>

MacroRecorder::MacroRecorder(QWidget *parent) :
    QDialog(parent),
    lInstructions("Record your macro;\npress \"STOP\" when finished.", this),
    endRecording("STOP", this),
    lastTime(std::chrono::system_clock::now())
{
    setWindowTitle("Recording Keys");
    endRecording.setFocusPolicy(Qt::NoFocus);

    setFixedSize(200, 100);
    lInstructions.setGeometry(0, 0, 200, 50);
    endRecording.setGeometry(80, 60, 40, 30);

    lInstructions.setAlignment(Qt::AlignCenter);

    installEventFilter(this);
    connect(&endRecording, SIGNAL(released()), this, SLOT(stopRecording()));
}

void MacroRecorder::showEvent(QShowEvent *event)
{
    keys.clear();
    event->accept();
}

bool MacroRecorder::eventFilter(QObject *watched, QEvent *event)
{
    // Record both press and release events
    if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) {
        auto eventData = reinterpret_cast<QKeyEvent*>(event);

        // Don't let a held down key send more than one press
        if (eventData->isAutoRepeat())
            return QObject::eventFilter(watched, event);

        auto k = eventData->key();
        auto mod = eventData->modifiers();

        if (k == Qt::Key_Control || k == Qt::Key_Alt ||
            k == Qt::Key_Shift)
            mod = Qt::NoModifier;

        // Prepare data
        Key key (k, mod);
        bool press = event->type() == QEvent::KeyPress;

        // Measure delay
        auto now = std::chrono::system_clock::now();
        auto delay = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTime);

        // Store data
        keys.emplace_back(key, press, delay);
        lastTime = now;
    }

    return QObject::eventFilter(watched, event);
}

void MacroRecorder::stopRecording(void)
{
    if (keys.size() > 1) {
        // Shift delays back one
        for (unsigned int i = 0; i < keys.size() - 1; i++)
            keys[i].delay = keys[i + 1].delay;
        keys.back().delay = std::chrono::milliseconds::zero();
    }

    emit recordFinished(keys);
    hide();
}

