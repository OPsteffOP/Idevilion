// Folder: Window

#pragma once
#ifdef BP_PLATFORM_ANDROID

#include "Window.h"

struct ANativeActivity;
struct ANativeWindow;

class AndroidWindow : public Window
{
public:
	AndroidWindow(const std::string& title, uint width, uint height, void* pUserData);

	virtual void Update() override;

	virtual void Close() override;

	virtual bool IsFocused() const override { return m_HasFocus; }
	virtual Point2f GetLocalMousePosition() const override { return Point2f(); } // TODO

private:
	static void OnCreate(ANativeActivity* pActivity, ANativeWindow* pNativeWindow);
	static void OnClose(ANativeActivity* pActivity, ANativeWindow* pNativeWindow);
	static void OnFocusChanged(ANativeActivity* pActivity, int isFocused);

private:
	bool m_HasFocus;
};

#endif