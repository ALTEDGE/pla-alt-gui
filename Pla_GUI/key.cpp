#include "key.h"

#include "macro.h"

// Windows-specific includes for sending keystrokes
#ifdef _WIN64

#include <windows.h>
#include <vector>

#else

// Use libxdo for Linux-based systems
extern "C" {
#include <xdo.h>
}
#undef KeyPress
#undef KeyRelease

static xdo_t *xdo = xdo_new(nullptr);

#endif // _WIN64



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
            text += "Ctrl";
        else if (key == Qt::Key_Alt)
            text += "Alt";
        else if (key == Qt::Key_Shift)
            text += "Shift";
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
// Creates an array of keys to send, including modifiers
#ifdef _WIN64
    // Fills an INPUT structure for the given key value
    auto createEvent = [&](INPUT& input, WORD wVk) {
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = wVk;
        input.ki.wScan = 0;
        input.ki.dwFlags = !press ? static_cast<DWORD>(KEYEVENTF_KEYUP) : 0;
        input.ki.time = 0;
        input.ki.dwExtraInfo = static_cast<ULONG_PTR>(GetMessageExtraInfo());
    };

    std::vector<INPUT> inputs;
    inputs.reserve(4);

    // Handle modifiers
    if (mod & Qt::ControlModifier) {
        inputs.emplace_back();
        createEvent(inputs.back(), VK_CONTROL);
    }
    if (mod & Qt::AltModifier) {
        inputs.emplace_back();
        createEvent(inputs.back(), VK_MENU);
    }
    if (mod & Qt::ShiftModifier) {
        inputs.emplace_back();
        createEvent(inputs.back(), VK_SHIFT);
    }

    // Handle key
    WORD vk = key;
    switch (key) {
    case Qt::Key_Control:
        vk = VK_CONTROL;
        break;
    case Qt::Key_Shift:
        vk = VK_SHIFT;
        break;
    case Qt::Key_Alt:
        vk = VK_MENU;
        break;
    case Qt::Key_Backspace:
        vk = VK_BACK;
        break;
    case Qt::Key_Tab:
        vk = VK_TAB;
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        vk =  VK_RETURN;
        break;
    case Qt::Key_Pause:
        vk = VK_PAUSE;
        break;
    case Qt::Key_Escape:
        vk = VK_ESCAPE;
        break;
    case Qt::Key_Space:
        vk = VK_SPACE;
        break;
    case Qt::Key_PageUp:
        vk = VK_PRIOR;
        break;
    case Qt::Key_PageDown:
        vk = VK_NEXT;
        break;
    case Qt::Key_End:
        vk = VK_END;
        break;
    case Qt::Key_Home:
        vk = VK_HOME;
        break;
    case Qt::Key_Left:
        vk = VK_LEFT;
        break;
    case Qt::Key_Up:
        vk = VK_UP;
        break;
    case Qt::Key_Right:
        vk = VK_RIGHT;
        break;
    case Qt::Key_Down:
        vk = VK_DOWN;
        break;
    case Qt::Key_Print:
        vk = VK_SNAPSHOT;
        break;
    case Qt::Key_Insert:
        vk = VK_INSERT;
        break;
    case Qt::Key_Delete:
        vk = VK_DELETE;
        break;
    case Qt::Key_F1:
        vk = VK_F1;
        break;
    case Qt::Key_F2:
        vk = VK_F2;
        break;
    case Qt::Key_F3:
        vk = VK_F3;
        break;
    case Qt::Key_F4:
        vk = VK_F4;
        break;
    case Qt::Key_F5:
        vk = VK_F5;
        break;
    case Qt::Key_F6:
        vk = VK_F6;
        break;
    case Qt::Key_F7:
        vk = VK_F7;
        break;
    case Qt::Key_F8:
        vk = VK_F8;
        break;
    case Qt::Key_F9:
        vk = VK_F9;
        break;
    case Qt::Key_F10:
        vk = VK_F10;
        break;
    case Qt::Key_F11:
        vk = VK_F11;
        break;
    case Qt::Key_F12:
        vk = VK_F12;
        break;
    }

    // Add key to array
    inputs.emplace_back();
    createEvent(inputs.back(), vk);

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
        text += (mod & Qt::ShiftModifier) ? (char)key : (char)tolower(key);
        break;
    }

    // Send the keystroke
    if (press)
        xdo_send_keysequence_window_down(xdo, CURRENTWINDOW, text.c_str(), 0);
    else
        xdo_send_keysequence_window_up(xdo, CURRENTWINDOW, text.c_str(), 0);

#endif // _WIN64
}
