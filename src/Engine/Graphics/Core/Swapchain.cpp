// Folder: Graphics/Core

#include "EnginePCH.h"
#include "Swapchain.h"

#include "RenderTarget.h"

Swapchain::Swapchain(uint width, uint height, WindowHandle pWindowHandle)
	: m_Width(width)
	, m_Height(height)
	, m_pWindowHandle(pWindowHandle)
	, m_pDefaultRenderTarget(nullptr)
{}

Swapchain::~Swapchain()
{
	SAFE_DELETE(m_pDefaultRenderTarget);
}