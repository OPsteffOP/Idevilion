// Folder: Window

#pragma once
#ifdef BP_PLATFORM_WINDOWS

#include "Window.h"

class WinWindow : public Window
{
public:
	WinWindow(const std::string& title, uint width, uint height, void* pUserData);

	virtual void Update() override;

	virtual void Close() override;

	virtual bool IsFocused() const override;
	virtual Point2f GetLocalMousePosition() const override;

private:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void OnUnfocus();
	void OnFocus();

private:
	static constexpr const char* WINDOW_CLASS_NAME = "BloodPath";
	static bool m_IsClassInitialized;
};

#endif