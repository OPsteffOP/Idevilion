// Folder: Window

#include "EnginePCH.h"
#include "AndroidWindow.h"

#ifdef BP_PLATFORM_ANDROID

#include <android\native_activity.h>

AndroidWindow::AndroidWindow(const std::string& title, uint width, uint height, void* pUserData)
	: Window(title, width, height, pUserData)
{
	ANativeActivity* pActivity = static_cast<ANativeActivity*>(pUserData);
	pActivity->instance = this;
	pActivity->callbacks->onNativeWindowCreated = OnCreate;
	pActivity->callbacks->onNativeWindowDestroyed = OnClose;
	pActivity->callbacks->onWindowFocusChanged = OnFocusChanged;
}

void AndroidWindow::Update()
{

}

void AndroidWindow::Close()
{
	if (m_OnCloseCallback != nullptr)
		m_OnCloseCallback(this);

	// TODO: actually close the window?
	m_pWindowHandle = nullptr;
}

void AndroidWindow::OnCreate(ANativeActivity* pActivity, ANativeWindow* pNativeWindow)
{
	m_pWindowHandle = pNativeWindow;
	// TODO: set size (m_Width, m_Height)
}

void AndroidWindow::OnClose(ANativeActivity* pActivity, ANativeWindow* pNativeWindow)
{
	AndroidWindow* pWindow = static_cast<AndroidWindow*>(pActivity->instance);
	pWindow->Close();
}

void AndroidWindow::OnFocusChanged(ANativeActivity* pActivity, int isFocused)
{
	AndroidWindow* pWindow = static_cast<AndroidWindow*>(pActivity->instance);
	pWindow->m_HasFocus = (bool)isFocused;

	if (pWindow->m_HasFocus)
	{
		if (pWindow->m_OnFocusCallback != nullptr)
			pWindow->m_OnFocusCallback(pWindow);
	}
	else
	{
		if (pWindow->m_OnUnfocusCallback != nullptr)
			pWindow->m_OnUnfocusCallback(pWindow);
	}
}

#endif