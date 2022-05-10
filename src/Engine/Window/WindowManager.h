// Folder: Window

#pragma once
#include "Window.h"

enum class WindowIdentifier
{
	MAIN_WINDOW
#if defined(DEV_BUILD) && defined(BP_PLATFORM_WINDOWS)
	, USER_PROVIDED_0
	, USER_PROVIDED_1
	, USER_PROVIDED_2
	, USER_PROVIDED_3
	, USER_PROVIDED_4
	, USER_PROVIDED_5
	, USER_PROVIDED_6
	, USER_PROVIDED_7
#endif

	, _COUNT
};

class WindowManager
{
public:
	static WindowManager* GetInstance();
	~WindowManager();

	Window* CreateWindow(WindowIdentifier identifier, const std::string& title, uint width, uint height);
	Window* CreateWindow(WindowIdentifier identifier, const std::string& title, uint width, uint height, void* pUserData);
	Window* GetWindow(WindowIdentifier identifier);
	
	void SetCurrentWindow(WindowIdentifier identifier);
	Window* GetCurrentWindow();
	WindowIdentifier GetCurrentWindowIdentifier() { return m_CurrentWindowIdentifier; }

private:
	WindowManager();

private:
	WindowIdentifier m_CurrentWindowIdentifier;
	Window* m_pWindows[(uint)WindowIdentifier::_COUNT];
};