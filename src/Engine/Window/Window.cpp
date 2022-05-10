// Folder: Window

#include "EnginePCH.h"
#include "Window.h"

#include "Swapchain.h"

Window::Window(const std::string& title, uint width, uint height, void* pUserData)
	: m_Title(title)
	, m_Width(width)
	, m_Height(height)
	, m_pWindowHandle(nullptr)
	, m_pSwapchain(nullptr)
{
	UNREFERENCED_PARAMETER(pUserData);
}

Window::~Window()
{
	SAFE_DELETE(m_pSwapchain);

	if (!IsClosed())
		Close();
}