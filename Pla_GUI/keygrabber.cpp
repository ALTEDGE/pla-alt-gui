#include "keygrabber.h"
#include <QKeyEvent>

#include "controller.h"
#include "macro.h"

KeyGrabber::KeyGrabber(QWidget *parent) :
    QDialog(parent),
    lInstructions("Press a key (or combination),\nor select a macro.", this),
    useNone("Clear Slot", this),
    useMacro("Use Macro", this),
    macroNames(this)
{
    setWindowTitle("Enter Key Press");

    // Don't let controls handle Space or Enter
    useNone.setFocusPolicy(Qt::NoFocus);
    useMacro.setFocusPolicy(Qt::NoFocus);
    macroNames.setFocusPolicy(Qt::NoFocus);

    // Set geometry
    setFixedSize(200, 100);
    lInstructions.setGeometry(0, 0, 200, 40);
    lInstructions.setAlignment(Qt::AlignCenter);
    useMacro.setGeometry(130, 40, 60, 20);
    useNone.setGeometry(70, 70, 60, 20);
    macroNames.setGeometry(10, 40, 110, 20);

    // Connect signals/slots
    connect(&useNone, SIGNAL(released()), this, SLOT(noneClicked()));
    connect(&useMacro, SIGNAL(released()), this, SLOT(macroClicked()));

    // Begin capturing window events
    installEventFilter(this);
}

void KeyGrabber::showEvent(QShowEvent *event)
{
    // Load macro list
    macroNames.clear();
    auto names = Macro::getNames();
    for (const auto& n : names)
        macroNames.addItem(n.c_str());

    event->accept();
}

bool KeyGrabber::eventFilter(QObject *watched, QEvent *event)
{
    // Escape can only be received on KeyPress, handle that here
    if (event->type() == QEvent::KeyPress) {
        auto press = reinterpret_cast<QKeyEvent*>(event);
        if (press->key() == Qt::Key_Escape) {
            emit keyPressed(Key(press->key(), press->modifiers()));
            hide();
            return true;
        }
    }

    // Catch key releases
    if (event->type() == QEvent::KeyRelease) {
        auto press = reinterpret_cast<QKeyEvent*>(event);

        auto k = press->key();
        auto mod = press->modifiers();

        // If only a modifier key is being pressed, make sure the modifier
        // field doesn't have the same key too
        if (k == Qt::Key_Control || k == Qt::Key_Alt ||
            k == Qt::Key_Shift)
            mod = Qt::NoModifier;

        emit keyPressed(Key(k, mod));

        // 'Close' this window
        hide();
        return true;
    }

    return QObject::eventFilter(watched, event);
}

void KeyGrabber::noneClicked(void)
{
    // Send empty key event
    emit keyPressed(Key());
    hide();
}

void KeyGrabber::macroClicked(void)
{
    // Send event with macro-key
    emit keyPressed(Key(macroNames.currentText().toStdString()));
    hide();
}
