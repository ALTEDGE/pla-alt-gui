#include "key.h"

#include "config.h"
#include "macro.h"

#include <thread>

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
#ifdef _WIN64
	INPUT input;
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = 0;
	input.ki.dwFlags = static_cast<DWORD>(!press ?
		(KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP) : KEYEVENTF_SCANCODE);
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;

	switch (key) {
	case Qt::Escape:
		input.ki.wScan = 0x01;
		break;
	case Qt::Key_1:
		input.ki.wScan = 0x02;
		break;
	case Qt::Key_2:
		input.ki.wScan = 0x03;
		break;
	case Qt::Key_3:
		input.ki.wScan = 0x04;
		break;
	case Qt::Key_4:
		input.ki.wScan = 0x05;
		break;
	case Qt::Key_5:
		input.ki.wScan = 0x06;
		break;
	case Qt::Key_6:
		input.ki.wScan = 0x07;
		break;
	case Qt::Key_7:
		input.ki.wScan = 0x08;
		break;
	case Qt::Key_8:
		input.ki.wScan = 0x09;
		break;
	case Qt::Key_9:
		input.ki.wScan = 0x0A;
		break;
	case Qt::Key_0:
		input.ki.wScan = 0x0B;
		break;
	case Qt::Key_Minus:
		input.ki.wScan = 0x0C;
		break;
	case Qt::Key_Equal:
		input.ki.wScan = 0x0D;
		break;
	case Qt::Key_Backspace:
		input.ki.wScan = 0x0E;
		break;
	case Qt::Key_Tab:
		input.ki.wScan = 0x0F;
		break;
	case Qt::Key_BracketLeft:
		input.ki.wScan = 0x1A;
		break;
	case Qt::Key_BracketRight:
		input.ki.wScan = 0x1B;
		break;
	case Qt::Return:
	case Qt::Enter:
		input.ki.wScan = 0x1C;
		break;
	case Qt::Key_Control:
		input.ki.wScan = 0x1D;
		break;
	case Qt::Key_Semicolon:
		input.ki.wScan = 0x27;
		break;
	case Qt::Key_Apostraphe:
		input.ki.wScan = 0x28;
		break;
	case Qt::Key_QuoteLeft:
		input.ki.wScan = 0x29;
		break;
	case Qt::Key_Shift:
		input.ki.wScan = 0x2A;
		break;
	case Qt::Key_Backslash:
		input.ki.wScan = 0x2B;
		break;
	case Qt::Key_Comma:
		input.ki.wScan = 0x33;
		break;
	case Qt::Key_Period:
		input.ki.wScan = 0x34;
		break;
	case Qt::Key_Slash:
		input.ki.wScan = 0x35;
		break;
	case Qt::Key_Alt:
		input.ki.wScan = 0x38;
		break;
	case Qt::Key_Space:
		input.ki.wScan = 0x39;
		break;
	case Qt::Key_F1:
		input.ki.wScan = 0x3B;
		break;
	case Qt::Key_F2:
		input.ki.wScan = 0x3C;
		break;
	case Qt::Key_F3:
		input.ki.wScan = 0x3D;
		break;
	case Qt::Key_F4:
		input.ki.wScan = 0x3E;
		break;
	case Qt::Key_F5:
		input.ki.wScan = 0x3F;
		break;
	case Qt::Key_F6:
		input.ki.wScan = 0x40;
		break;
	case Qt::Key_F7:
		input.ki.wScan = 0x41;
		break;
	case Qt::Key_F8:
		input.ki.wScan = 0x42;
		break;
	case Qt::Key_F9:
		input.ki.wScan = 0x43;
		break;
	case Qt::Key_F10:
		input.ki.wScan = 0x44;
		break;
	case Qt::Key_F11:
		input.ki.wScan = 0x57;
		break;
	case Qt::Key_F12:
		input.ki.wScan = 0x58;
		break;
	case Qt::Key_A:
		input.ki.wScan = 0x1E;
		break;
	case Qt::Key_B:
		input.ki.wScan = 0x30;
		break;
	case Qt::Key_C:
		input.ki.wScan = 0x2E;
		break;
	case Qt::Key_D:
		input.ki.wScan = 0x20;
		break;
	case Qt::Key_E:
		input.ki.wScan = 0x12;
		break;
	case Qt::Key_F:
		input.ki.wScan = 0x21;
		break;
	case Qt::Key_G:
		input.ki.wScan = 0x22;
		break;
	case Qt::Key_H:
		input.ki.wScan = 0x23;
		break;
	case Qt::Key_I:
		input.ki.wScan = 0x17;
		break;
	case Qt::Key_J:
		input.ki.wScan = 0x24;
		break;
	case Qt::Key_K:
		input.ki.wScan = 0x25;
		break;
	case Qt::Key_L:
		input.ki.wScan = 0x26;
		break;
	case Qt::Key_M:
		input.ki.wScan = 0x32;
		break;
	case Qt::Key_N:
		input.ki.wScan = 0x31;
		break;
	case Qt::Key_O:
		input.ki.wScan = 0x18;
		break;
	case Qt::Key_P:
		input.ki.wScan = 0x19;
		break;
	case Qt::Key_Q:
		input.ki.wScan = 0x10;
		break;
	case Qt::Key_R:
		input.ki.wScan = 0x13;
		break;
	case Qt::Key_S:
		input.ki.wScan = 0x1F;
		break;
	case Qt::Key_T:
		input.ki.wScan = 0x14;
		break;
	case Qt::Key_U:
		input.ki.wScan = 0x16;
		break;
	case Qt::Key_V:
		input.ki.wScan = 0x2F;
		break;
	case Qt::Key_W:
		input.ki.wScan = 0x11;
		break;
	case Qt::Key_Y:
		input.ki.wScan = 0x15;
		break;
	case Qt::Key_X:
		input.ki.wScan = 0x2D;
		break;
	case Qt::Key_Z:
		input.ki.wScan = 0x2C;
		break;
	default:
		input.ki.wScan = 0;
		break;
	}

	// Send the keystrokes
	SendInput(1, &input, sizeof(INPUT));

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
		text += static_cast<char>((mod & Qt::ShiftModifier) ? key : tolower(key));
		break;
	}

	// Send the keystroke
	if (press)
		xdo_send_keysequence_window_down(xdo, CURRENTWINDOW, text.c_str(), 0);
	else
		xdo_send_keysequence_window_up(xdo, CURRENTWINDOW, text.c_str(), 0);

#endif // _WIN64

	std::this_thread::sleep_for(config::InputSendDelay);
}
