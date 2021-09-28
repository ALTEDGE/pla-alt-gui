#include "key.h"

#include "config.h"
#include "macro.h"

#include <thread>

// Windows-specific includes for sending keystrokes
#ifdef PLA_WINDOWS

typedef struct IUnknown IUnknown;
#include <Windows.h>
#include <vector>

#else

// Use libxdo for Linux-based systems
extern "C" {
#include <xdo.h>
}
#undef KeyPress
#undef KeyRelease

static xdo_t *xdo = xdo_new(nullptr);

#endif // PLA_WINDOWS



Key::Key(int k, Qt::KeyboardModifiers m) :
    key(k),
    mod(m)
{

}

Key::Key(const std::string &macroName) :
    key(-1),
    mod(Qt::NoModifier),
    macro(macroName)
{

}

Key::Key(const QSettings &settings) :
    key(settings.value("key", -1).toInt())
{
    macro = settings.value("macro").toString().toStdString();
    if (macro.empty()) {
        mod = static_cast<Qt::KeyboardModifiers>(settings.value(
            "mod", static_cast<int>(Qt::NoModifier)).toInt());
    } else {
        key = -1;
    }
}

bool Key::operator==(const Key& other) const
{
    return key == other.key && mod == other.mod && macro == other.macro;
}

bool Key::operator!=(const Key& other) const
{
    return !(*this == other);
}

bool Key::isValid(void) const
{
    // Only valid if a key is set but no macro, or a macro is set but no key
    return (key != -1) ^ Macro::exists(macro);
}

QString Key::toString(void) const
{
    if (!isValid())
        return "No binding";

    QString text;
    if (macro.empty()) {
        text = "Key: ";

        // Add modifiers
        if (mod & Qt::ControlModifier)
            text += "Ctrl + ";
        if (mod & Qt::AltModifier)
            text += "Alt + ";
        if (mod & Qt::ShiftModifier)
            text += "Shift + ";

        if (key == Qt::Key_Control)
            text += "L Ctrl";
        else if (key == Qt::Key_Alt)
            text += "Alt";
        else if (key == Qt::Key_Shift)
            text += "L Shift";
        else
            text += QKeySequence(key).toString();
    } else {
        text = "Macro: ";
        text += macro.c_str();
    }

    return text;
}

void Key::save(QSettings &settings) const
{
    if (Macro::exists(macro)) {
        settings.setValue("macro", macro.c_str());
    } else {
        settings.setValue("key", key);
        settings.setValue("mod", static_cast<int>(mod));
        settings.setValue("macro", "");
    }
}

void Key::fire(bool press) const
{
    if (!isValid())
        return;

    // Fire the macro if it exists
    if (!macro.empty()) {
        if (press)
            Macro::fire(macro);
        return;
    }

    // Otherwise, a key action:

// Fire code for Windows
#ifdef PLA_WINDOWS
    auto createEvent = [&](INPUT& input, WORD wScan) {
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = 0;
        input.ki.wScan = wScan;
        input.ki.dwFlags = static_cast<DWORD>(!press ?
            (KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE) : KEYEVENTF_SCANCODE);
        input.ki.time = 0;
        input.ki.dwExtraInfo = static_cast<ULONG_PTR>(GetMessageExtraInfo());
    };

    std::vector<INPUT> inputs;
    inputs.reserve(4);

    // Handle modifiers
    if (mod & Qt::ControlModifier) {
        inputs.emplace_back();
        createEvent(inputs.back(), 0x1D);
    }
    if (mod & Qt::AltModifier) {
        inputs.emplace_back();
        createEvent(inputs.back(), 0x38);
    }
    if (mod & Qt::ShiftModifier) {
        inputs.emplace_back();
        createEvent(inputs.back(), 0x2A);
    }

    WORD scan;
    switch (key) {
    case Qt::Key_Escape:
        scan = 0x01;
        break;
    case Qt::Key_1:
        scan = 0x02;
        break;
    case Qt::Key_2:
        scan = 0x03;
        break;
    case Qt::Key_3:
        scan = 0x04;
        break;
    case Qt::Key_4:
        scan = 0x05;
        break;
    case Qt::Key_5:
        scan = 0x06;
        break;
    case Qt::Key_6:
        scan = 0x07;
        break;
    case Qt::Key_7:
        scan = 0x08;
        break;
    case Qt::Key_8:
        scan = 0x09;
        break;
    case Qt::Key_9:
        scan = 0x0A;
        break;
    case Qt::Key_0:
        scan = 0x0B;
        break;
    case Qt::Key_Minus:
        scan = 0x0C;
        break;
    case Qt::Key_Equal:
        scan = 0x0D;
        break;
    case Qt::Key_Backspace:
        scan = 0x0E;
        break;
    case Qt::Key_Tab:
        scan = 0x0F;
        break;
    case Qt::Key_BracketLeft:
        scan = 0x1A;
        break;
    case Qt::Key_BracketRight:
        scan = 0x1B;
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        scan = 0x1C;
        break;
    case Qt::Key_Control:
        scan = 0x1D;
        break;
    case Qt::Key_Semicolon:
        scan = 0x27;
        break;
    case Qt::Key_Apostrophe:
        scan = 0x28;
        break;
    case Qt::Key_QuoteLeft:
        scan = 0x29;
        break;
    case Qt::Key_Shift:
        scan = 0x2A;
        break;
    case Qt::Key_Backslash:
        scan = 0x2B;
        break;
    case Qt::Key_Comma:
        scan = 0x33;
        break;
    case Qt::Key_Period:
        scan = 0x34;
        break;
    case Qt::Key_Slash:
        scan = 0x35;
        break;
    case Qt::Key_Alt:
        scan = 0x38;
        break;
    case Qt::Key_Space:
        scan = 0x39;
        break;
    case Qt::Key_F1:
        scan = 0x3B;
        break;
    case Qt::Key_F2:
        scan = 0x3C;
        break;
    case Qt::Key_F3:
        scan = 0x3D;
        break;
    case Qt::Key_F4:
        scan = 0x3E;
        break;
    case Qt::Key_F5:
        scan = 0x3F;
        break;
    case Qt::Key_F6:
        scan = 0x40;
        break;
    case Qt::Key_F7:
        scan = 0x41;
        break;
    case Qt::Key_F8:
        scan = 0x42;
        break;
    case Qt::Key_F9:
        scan = 0x43;
        break;
    case Qt::Key_F10:
        scan = 0x44;
        break;
    case Qt::Key_F11:
        scan = 0x57;
        break;
    case Qt::Key_F12:
        scan = 0x58;
        break;
    case Qt::Key_A:
        scan = 0x1E;
        break;
    case Qt::Key_B:
        scan = 0x30;
        break;
    case Qt::Key_C:
        scan = 0x2E;
        break;
    case Qt::Key_D:
        scan = 0x20;
        break;
    case Qt::Key_E:
        scan = 0x12;
        break;
    case Qt::Key_F:
        scan = 0x21;
        break;
    case Qt::Key_G:
        scan = 0x22;
        break;
    case Qt::Key_H:
        scan = 0x23;
        break;
    case Qt::Key_I:
        scan = 0x17;
        break;
    case Qt::Key_J:
        scan = 0x24;
        break;
    case Qt::Key_K:
        scan = 0x25;
        break;
    case Qt::Key_L:
        scan = 0x26;
        break;
    case Qt::Key_M:
        scan = 0x32;
        break;
    case Qt::Key_N:
        scan = 0x31;
        break;
    case Qt::Key_O:
        scan = 0x18;
        break;
    case Qt::Key_P:
        scan = 0x19;
        break;
    case Qt::Key_Q:
        scan = 0x10;
        break;
    case Qt::Key_R:
        scan = 0x13;
        break;
    case Qt::Key_S:
        scan = 0x1F;
        break;
    case Qt::Key_T:
        scan = 0x14;
        break;
    case Qt::Key_U:
        scan = 0x16;
        break;
    case Qt::Key_V:
        scan = 0x2F;
        break;
    case Qt::Key_W:
        scan = 0x11;
        break;
    case Qt::Key_Y:
        scan = 0x15;
        break;
    case Qt::Key_X:
        scan = 0x2D;
        break;
    case Qt::Key_Z:
        scan = 0x2C;
        break;
    case Qt::Key_Insert:
        scan = 0x52;
        break;
    case Qt::Key_Delete:
        scan = 0x53;
        break;
    case Qt::Key_Home:
        scan = 0x47;
        break;
    case Qt::Key_End:
        scan = 0x4F;
        break;
    case Qt::Key_PageUp:
        scan = 0x49;
        break;
    case Qt::Key_PageDown:
        scan = 0x51;
        break;
    case Qt::Key_Up:
        scan = 0x48;
        break;
    case Qt::Key_Down:
        scan = 0x50;
        break;
    case Qt::Key_Left:
        scan = 0x4B;
        break;
    case Qt::Key_Right:
        scan = 0x4D;
        break;
    default:
        scan = 0;
        break;
    }

    if (scan != 0) {
        inputs.emplace_back();
        createEvent(inputs.back(), scan);
    }

    // Send the keystrokes
    SendInput(static_cast<UINT>(inputs.size()), inputs.data(), sizeof(INPUT));

#else
    // Fire code for Linux-based OSes

    static std::string text (20, '\0');

    // Add modifiers
    text.clear();
    if (mod & Qt::ControlModifier)
        text += "ctrl+";
    if (mod & Qt::AltModifier)
        text += "alt+";
    if (mod & Qt::ShiftModifier)
        text += "Shift+";

    // Add the key
    switch (key) {
    case Qt::Key_Control:
        text += "ctrl";
        break;
    case Qt::Key_Shift:
        text += "Shift";
        break;
    case Qt::Key_Alt:
        text += "alt";
        break;
    case Qt::Key_Tab:
        text += "Tab";
        break;
    case Qt::Key_Backspace:
        text += "BackSpace";
        break;
    case Qt::Key_Return:
        text += "Return";
        break;
    case Qt::Key_Space:
        text += "space";
        break;
    case Qt::Key_Up:
        text += "Up";
        break;
    case Qt::Key_Down:
        text += "Down";
        break;
    case Qt::Key_Left:
        text += "Left";
        break;
    case Qt::Key_Right:
        text += "Right";
        break;
    case Qt::Key_F1:
        text += "F1";
        break;
    case Qt::Key_F2:
        text += "F2";
        break;
    case Qt::Key_F3:
        text += "F3";
        break;
    case Qt::Key_F4:
        text += "F4";
        break;
    case Qt::Key_F5:
        text += "F5";
        break;
    case Qt::Key_F6:
        text += "F6";
        break;
    case Qt::Key_F7:
        text += "F7";
        break;
    case Qt::Key_F8:
        text += "F8";
        break;
    case Qt::Key_F9:
        text += "F9";
        break;
    case Qt::Key_F10:
        text += "F10";
        break;
    case Qt::Key_F11:
        text += "F11";
        break;
    case Qt::Key_F12:
        text += "F12";
        break;
    default:
        text += static_cast<char>((mod & Qt::ShiftModifier) ? tolower(key) : key);
        break;
    }

    // Send the keystroke
    if (press)
        xdo_send_keysequence_window_down(xdo, CURRENTWINDOW, text.c_str(), 0);
    else
        xdo_send_keysequence_window_up(xdo, CURRENTWINDOW, text.c_str(), 0);

#endif // PLA_WINDOWS

    std::this_thread::sleep_for(config::InputSendDelay);
}
