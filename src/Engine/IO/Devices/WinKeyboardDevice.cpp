// Folder: IO/Devices

#include "EnginePCH.h"
#include "WinKeyboardDevice.h"

#ifdef BP_PLATFORM_WINDOWS

void WinKeyboardDevice::Update()
{
	// Checking if the focused window is one of our windows
	if (GetWindowThreadProcessId(GetFocus(), NULL) == GetCurrentThreadId())
	{
		for (uint i = 0; i < (uint)InputAction::_COUNT_KEYBOARD; ++i)
		{
			InputAction action = (InputAction)i;
			SetState(action, (bool)GetAsyncKeyState(ConvertInputAction(action)));
		}
	}
}

uint WinKeyboardDevice::ConvertInputAction(InputAction action) const
{
	switch (action)
	{
	case InputAction::KEYBOARD_BACKSPACE:
		return VK_BACK;
	case InputAction::KEYBOARD_TAB:
		return VK_TAB;
	case InputAction::KEYBOARD_CLEAR:
		return VK_CLEAR;
	case InputAction::KEYBOARD_RETURN:
		return VK_RETURN;
	case InputAction::KEYBOARD_PAUSE:
		return VK_PAUSE;
	case InputAction::KEYBOARD_CAPSLOCK:
		return VK_CAPITAL;
	case InputAction::KEYBOARD_ESCAPE:
		return VK_ESCAPE;
	case InputAction::KEYBOARD_SPACE:
		return VK_SPACE;
	case InputAction::KEYBOARD_PAGE_UP:
		return VK_PRIOR;
	case InputAction::KEYBOARD_PAGE_DOWN:
		return VK_NEXT;
	case InputAction::KEYBOARD_END:
		return VK_END;
	case InputAction::KEYBOARD_HOME:
		return VK_HOME;
	case InputAction::KEYBOARD_LEFT_ARROW:
		return VK_LEFT;
	case InputAction::KEYBOARD_RIGHT_ARROW:
		return VK_RIGHT;
	case InputAction::KEYBOARD_UP_ARROW:
		return VK_UP;
	case InputAction::KEYBOARD_DOWN_ARROW:
		return VK_DOWN;
	case InputAction::KEYBOARD_SELECT:
		return VK_SELECT;
	case InputAction::KEYBOARD_PRINT:
		return VK_PRINT;
	case InputAction::KEYBOARD_EXECUTE:
		return VK_EXECUTE;
	case InputAction::KEYBOARD_PRINT_SCREEN:
		return VK_SNAPSHOT;
	case InputAction::KEYBOARD_INSERT:
		return VK_INSERT;
	case InputAction::KEYBOARD_DELETE:
		return VK_DELETE;
	case InputAction::KEYBOARD_HELP:
		return VK_HELP;
	case InputAction::KEYBOARD_0:
		return 0x30;
	case InputAction::KEYBOARD_1:
		return 0x31;
	case InputAction::KEYBOARD_2:
		return 0x32;
	case InputAction::KEYBOARD_3:
		return 0x33;
	case InputAction::KEYBOARD_4:
		return 0x34;
	case InputAction::KEYBOARD_5:
		return 0x35;
	case InputAction::KEYBOARD_6:
		return 0x36;
	case InputAction::KEYBOARD_7:
		return 0x37;
	case InputAction::KEYBOARD_8:
		return 0x38;
	case InputAction::KEYBOARD_9:
		return 0x39;
	case InputAction::KEYBOARD_A:
		return 0x41;
	case InputAction::KEYBOARD_B:
		return 0x42;
	case InputAction::KEYBOARD_C:
		return 0x43;
	case InputAction::KEYBOARD_D:
		return 0x44;
	case InputAction::KEYBOARD_E:
		return 0x45;
	case InputAction::KEYBOARD_F:
		return 0x46;
	case InputAction::KEYBOARD_G:
		return 0x47;
	case InputAction::KEYBOARD_H:
		return 0x48;
	case InputAction::KEYBOARD_I:
		return 0x49;
	case InputAction::KEYBOARD_J:
		return 0x4A;
	case InputAction::KEYBOARD_K:
		return 0x4B;
	case InputAction::KEYBOARD_L:
		return 0x4C;
	case InputAction::KEYBOARD_M:
		return 0x4D;
	case InputAction::KEYBOARD_N:
		return 0x4E;
	case InputAction::KEYBOARD_O:
		return 0x4F;
	case InputAction::KEYBOARD_P:
		return 0x50;
	case InputAction::KEYBOARD_Q:
		return 0x51;
	case InputAction::KEYBOARD_R:
		return 0x52;
	case InputAction::KEYBOARD_S:
		return 0x53;
	case InputAction::KEYBOARD_T:
		return 0x54;
	case InputAction::KEYBOARD_U:
		return 0x55;
	case InputAction::KEYBOARD_V:
		return 0x56;
	case InputAction::KEYBOARD_W:
		return 0x57;
	case InputAction::KEYBOARD_X:
		return 0x58;
	case InputAction::KEYBOARD_Y:
		return 0x59;
	case InputAction::KEYBOARD_Z:
		return 0x5A;
	case InputAction::KEYBOARD_LWINDOWS:
		return VK_LWIN;
	case InputAction::KEYBOARD_RWINDOWS:
		return VK_RWIN;
	case InputAction::KEYBOARD_APPS:
		return VK_APPS;
	case InputAction::KEYBOARD_SLEEP:
		return VK_SLEEP;
	case InputAction::KEYBOARD_NUMPAD0:
		return VK_NUMPAD0;
	case InputAction::KEYBOARD_NUMPAD1:
		return VK_NUMPAD1;
	case InputAction::KEYBOARD_NUMPAD2:
		return VK_NUMPAD2;
	case InputAction::KEYBOARD_NUMPAD3:
		return VK_NUMPAD3;
	case InputAction::KEYBOARD_NUMPAD4:
		return VK_NUMPAD4;
	case InputAction::KEYBOARD_NUMPAD5:
		return VK_NUMPAD5;
	case InputAction::KEYBOARD_NUMPAD6:
		return VK_NUMPAD6;
	case InputAction::KEYBOARD_NUMPAD7:
		return VK_NUMPAD7;
	case InputAction::KEYBOARD_NUMPAD8:
		return VK_NUMPAD8;
	case InputAction::KEYBOARD_NUMPAD9:
		return VK_NUMPAD9;
	case InputAction::KEYBOARD_MULTIPLY:
		return VK_MULTIPLY;
	case InputAction::KEYBOARD_ADD:
		return VK_ADD;
	case InputAction::KEYBOARD_SEPARATOR:
		return VK_SEPARATOR;
	case InputAction::KEYBOARD_SUBTRACT:
		return VK_SUBTRACT;
	case InputAction::KEYBOARD_DECIMAL:
		return VK_DECIMAL;
	case InputAction::KEYBOARD_DIVIDE:
		return VK_DIVIDE;
	case InputAction::KEYBOARD_F1:
		return VK_F1;
	case InputAction::KEYBOARD_F2:
		return VK_F2;
	case InputAction::KEYBOARD_F3:
		return VK_F3;
	case InputAction::KEYBOARD_F4:
		return VK_F4;
	case InputAction::KEYBOARD_F5:
		return VK_F5;
	case InputAction::KEYBOARD_F6:
		return VK_F6;
	case InputAction::KEYBOARD_F7:
		return VK_F7;
	case InputAction::KEYBOARD_F8:
		return VK_F8;
	case InputAction::KEYBOARD_F9:
		return VK_F9;
	case InputAction::KEYBOARD_F10:
		return VK_F10;
	case InputAction::KEYBOARD_F11:
		return VK_F11;
	case InputAction::KEYBOARD_F12:
		return VK_F12;
	case InputAction::KEYBOARD_F13:
		return VK_F13;
	case InputAction::KEYBOARD_F14:
		return VK_F14;
	case InputAction::KEYBOARD_F15:
		return VK_F15;
	case InputAction::KEYBOARD_F16:
		return VK_F16;
	case InputAction::KEYBOARD_F17:
		return VK_F17;
	case InputAction::KEYBOARD_F18:
		return VK_F18;
	case InputAction::KEYBOARD_F19:
		return VK_F19;
	case InputAction::KEYBOARD_F20:
		return VK_F20;
	case InputAction::KEYBOARD_F21:
		return VK_F21;
	case InputAction::KEYBOARD_F22:
		return VK_F22;
	case InputAction::KEYBOARD_F23:
		return VK_F23;
	case InputAction::KEYBOARD_F24:
		return VK_F24;
	case InputAction::KEYBOARD_NUMLOCK:
		return VK_NUMLOCK;
	case InputAction::KEYBOARD_SCROLLLOCK:
		return VK_SCROLL;
	case InputAction::KEYBOARD_LSHIFT:
		return VK_LSHIFT;
	case InputAction::KEYBOARD_RSHIFT:
		return VK_RSHIFT;
	case InputAction::KEYBOARD_LCONTROL:
		return VK_LCONTROL;
	case InputAction::KEYBOARD_RCONTROL:
		return VK_RCONTROL;
	case InputAction::KEYBOARD_LALT:
		return VK_LMENU;
	case InputAction::KEYBOARD_RALT:
		return VK_RMENU;
	case InputAction::KEYBOARD_BROWSER_BACK:
		return VK_BROWSER_BACK;
	case InputAction::KEYBOARD_BROWSER_FORWARD:
		return VK_BROWSER_FORWARD;
	case InputAction::KEYBOARD_BROWSER_REFRESH:
		return VK_BROWSER_REFRESH;
	case InputAction::KEYBOARD_BROWSER_STOP:
		return VK_BROWSER_STOP;
	case InputAction::KEYBOARD_BROWSER_SEARCH:
		return VK_BROWSER_SEARCH;
	case InputAction::KEYBOARD_BROWSER_FAVORITES:
		return VK_BROWSER_FAVORITES;
	case InputAction::KEYBOARD_BROWSER_HOME:
		return VK_BROWSER_HOME;
	case InputAction::KEYBOARD_VOLUME_MUTE:
		return VK_VOLUME_MUTE;
	case InputAction::KEYBOARD_VOLUME_DOWN:
		return VK_VOLUME_DOWN;
	case InputAction::KEYBOARD_VOLUME_UP:
		return VK_VOLUME_UP;
	case InputAction::KEYBOARD_MEDIA_NEXT:
		return VK_MEDIA_NEXT_TRACK;
	case InputAction::KEYBOARD_MEDIA_PREVIOUS:
		return VK_MEDIA_PREV_TRACK;
	case InputAction::KEYBOARD_MEDIA_STOP:
		return VK_MEDIA_STOP;
	case InputAction::KEYBOARD_MEDIA_PLAY_PAUSE:
		return VK_MEDIA_PLAY_PAUSE;
	case InputAction::KEYBOARD_LAUNCH_MAIL:
		return VK_LAUNCH_MAIL;
	case InputAction::KEYBOARD_LAUNCH_MEDIA_SELECT:
		return VK_LAUNCH_MEDIA_SELECT;
	case InputAction::KEYBOARD_LAUNCH_APP1:
		return VK_LAUNCH_APP1;
	case InputAction::KEYBOARD_LAUNCH_APP2:
		return VK_LAUNCH_APP2;
	case InputAction::KEYBOARD_OEM_COLON_SEMICOLON:
		return VK_OEM_1;
	case InputAction::KEYBOARD_OEM_PLUS:
		return VK_OEM_PLUS;
	case InputAction::KEYBOARD_OEM_COMMA:
		return VK_OEM_COMMA;
	case InputAction::KEYBOARD_OEM_MINUS:
		return VK_OEM_MINUS;
	case InputAction::KEYBOARD_OEM_PERIOD:
		return VK_OEM_PERIOD;
	case InputAction::KEYBOARD_OEM_SLASH_QUESTIONMARK:
		return VK_OEM_2;
	case InputAction::KEYBOARD_OEM_TILDE:
		return VK_OEM_3;
	case InputAction::KEYBOARD_OEM_BRACKETS_OPEN:
		return VK_OEM_4;
	case InputAction::KEYBOARD_OEM_BACKSLASH_PIPE:
		return VK_OEM_5;
	case InputAction::KEYBOARD_OEM_BRACKETS_CLOSE:
		return VK_OEM_6;
	case InputAction::KEYBOARD_OEM_QUOTES:
		return VK_OEM_7;
	case InputAction::KEYBOARD_OEM_ANGLE_BRACKET:
		return VK_OEM_102;
	}

	LOG_ERROR("Unknown InputAction keyboard key, key=%d", (uint)action);
	return UINT_MAX;
}

#endif