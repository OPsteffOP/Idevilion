// Folder: IO/Devices

#include "EnginePCH.h"
#include "WinMouseDevice.h"

#ifdef BP_PLATFORM_WINDOWS

WinMouseDevice::WinMouseDevice(InputDeviceIdentifier identifier)
	: MouseInputDevice(identifier)
{}

void WinMouseDevice::Update()
{
	CURSORINFO info;
	info.cbSize = sizeof(info);
	GetCursorInfo(&info);

	m_IsCursorVisible = info.flags & CURSOR_SHOWING;
	m_PreviousMousePosition.x = m_MousePosition.x;
	m_PreviousMousePosition.y = m_MousePosition.y;
	m_MousePosition.x = (float)info.ptScreenPos.x;
	m_MousePosition.y = (float)info.ptScreenPos.y;

	// Checking if the focused window is one of our windows
	if (GetWindowThreadProcessId(GetFocus(), NULL) == GetCurrentThreadId())
	{
		uint leftMouseButton = VK_LBUTTON;
		uint rightMouseButton = VK_RBUTTON;
		if (GetSystemMetrics(SM_SWAPBUTTON))
			std::swap(leftMouseButton, rightMouseButton);

		SetState((uint)MouseControl::MOUSE_LEFT_BUTTON, (bool)GetAsyncKeyState(leftMouseButton));
		SetState((uint)MouseControl::MOUSE_RIGHT_BUTTON, (bool)GetAsyncKeyState(rightMouseButton));
		SetState((uint)MouseControl::MOUSE_MIDDLE_BUTTON, (bool)GetAsyncKeyState(VK_MBUTTON));
		SetState((uint)MouseControl::MOUSE_SIDE_BUTTON_ONE, (bool)GetAsyncKeyState(VK_XBUTTON1));
		SetState((uint)MouseControl::MOUSE_SIDE_BUTTON_TWO, (bool)GetAsyncKeyState(VK_XBUTTON2));
	}
}

bool WinMouseDevice::IsCursorVisible() const
{
	return m_IsCursorVisible;
}

void WinMouseDevice::SetCursorVisible(bool isVisible)
{
	ShowCursor((BOOL)isVisible);
}

#endif