// Folder: Window

#include "EnginePCH.h"
#include "WindowManager.h"

#include "WinWindow.h"
#include "AndroidWindow.h"

WindowManager* WindowManager::GetInstance()
{
	static WindowManager* pInstance = nullptr;
	if (pInstance == nullptr)
	{
		ALLOW_NEXT_ALLOCATION_LEAK();
		pInstance = new WindowManager();
	}

	return pInstance;
}

WindowManager::WindowManager()
	: m_CurrentWindowIdentifier(WindowIdentifier::MAIN_WINDOW)
{
	std::fill(std::begin(m_pWindows), std::end(m_pWindows), nullptr);
}

WindowManager::~WindowManager()
{
	for (uint i = 0; i < (uint)WindowIdentifier::_COUNT; ++i)
	{
		SAFE_DELETE(m_pWindows[i]);
	}
}

Window* WindowManager::CreateWindow(WindowIdentifier identifier, const std::string& title, uint width, uint height)
{
	return CreateWindow(identifier, title, width, height, nullptr);
}

Window* WindowManager::CreateWindow(WindowIdentifier identifier, const std::string& title, uint width, uint height, void* pUserData)
{
	if (m_pWindows[(uint)identifier] != nullptr)
	{
		LOG_DEBUG("Creating a window for an identifier that already exists, deleting previous window and overwritting with new one, identifier=%d", (uint)identifier);
		SAFE_DELETE(m_pWindows[(uint)identifier]);
	}

#ifdef BP_PLATFORM_WINDOWS
	Window* pWindow = new WinWindow(title, width, height, pUserData);
	m_pWindows[(uint)identifier] = pWindow;
	return pWindow;
#elif BP_PLATFORM_ANDROID
	Window* pWindow = new AndroidWindow(title, width, height, pUserData);
	m_pWindows[(uint)identifier] = pWindow;
	return pWindow;
#else
	#error Window isnt supported for the current platform
#endif
}

Window* WindowManager::GetWindow(WindowIdentifier identifier)
{
	return m_pWindows[(uint)identifier];
}

void WindowManager::SetCurrentWindow(WindowIdentifier identifier)
{
	m_CurrentWindowIdentifier = identifier;
}

Window* WindowManager::GetCurrentWindow()
{
	return GetWindow(m_CurrentWindowIdentifier);
}