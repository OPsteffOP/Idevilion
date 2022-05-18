// Folder: Window

#include "EnginePCH.h"
#include "WinWindow.h"

#ifdef BP_PLATFORM_WINDOWS

#include "InputManager.h"

bool WinWindow::m_IsClassInitialized = false;

LRESULT CALLBACK WinWindow::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(Window*)((LPCREATESTRUCT)lParam)->lpCreateParams);
		break;
	case WM_CLOSE:
		reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA))->Close();
		break;

	case WM_KILLFOCUS:
		reinterpret_cast<WinWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA))->OnUnfocus();
		break;
	case WM_SETFOCUS:
		reinterpret_cast<WinWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA))->OnFocus();
		break;
	}

	InputManager::WindowProc(hwnd, msg, wParam, lParam);

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

WinWindow::WinWindow(const std::string& title, uint width, uint height, void* pUserData)
	: Window(title, width, height, pUserData)
{
	const std::wstring windowClassNameWStr = Utils::StringToWideString(WINDOW_CLASS_NAME);
	const std::wstring titleWStr = Utils::StringToWideString(title);

	if (!m_IsClassInitialized)
	{
		WNDCLASS wc{};
		wc.lpfnWndProc = WindowProc;
		wc.hInstance = GetModuleHandle(NULL);
		wc.lpszClassName = windowClassNameWStr.c_str();

		if (RegisterClass(&wc) == 0)
		{
			LOG_ERROR("Failed to register window class");
			return;
		}

		m_IsClassInitialized = true;
	}

	RECT screenRect;
	HWND pScreenHandle = GetDesktopWindow();
	GetClientRect(pScreenHandle, &screenRect);

	RECT rect{ 0, 0, (LONG)width, (LONG)height };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	uint actualWindowWidth = rect.right - rect.left;
	uint actualWindowHeight = rect.bottom - rect.top;

	m_pWindowHandle = CreateWindowEx(
		0,
		windowClassNameWStr.c_str(),
		titleWStr.c_str(),
		WS_OVERLAPPEDWINDOW,
		(screenRect.right / 2) - (actualWindowWidth / 2), (screenRect.bottom / 2) - (actualWindowHeight / 2),
		actualWindowWidth, actualWindowHeight,
		NULL,
		NULL,
		GetModuleHandle(NULL),
		this
	);

	ShowWindow((HWND)m_pWindowHandle, SW_SHOW);
	UpdateWindow((HWND)m_pWindowHandle);
}

void WinWindow::Update()
{
	MSG msg;
	PeekMessage(&msg, (HWND)m_pWindowHandle, 0, 0, PM_REMOVE);
	TranslateMessage(&msg);
	DispatchMessage(&msg);
}

void WinWindow::Close()
{
	if (m_OnCloseCallback != nullptr)
		m_OnCloseCallback(this);

	DestroyWindow((HWND)m_pWindowHandle);
	m_pWindowHandle = nullptr;
}

bool WinWindow::IsFocused() const
{
	return GetFocus() == (HWND)m_pWindowHandle;
}

Point2f WinWindow::GetLocalMousePosition() const
{
	MouseInputDevice* pMouseDevice = static_cast<MouseInputDevice*>(InputManager::GetInstance()->GetDevice(InputDeviceIdentifier::MOUSE));
	const Point2f& screenMousePosition = pMouseDevice->GetMousePosition();

	POINT point;
	point.x = (LONG)screenMousePosition.x;
	point.y = (LONG)screenMousePosition.y;
	ScreenToClient((HWND)m_pWindowHandle, &point);

	return Point2f{ (float)point.x, (float)point.y };
}

void WinWindow::OnUnfocus()
{
	if (m_OnUnfocusCallback != nullptr)
		m_OnUnfocusCallback(this);
}

void WinWindow::OnFocus()
{
	if (m_OnFocusCallback != nullptr)
		m_OnFocusCallback(this);
}

#endif