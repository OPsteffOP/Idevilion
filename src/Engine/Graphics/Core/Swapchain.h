// Folder: Graphics/Core

#pragma once

class RenderTarget;

class Swapchain
{
public:
	Swapchain(uint width, uint height, WindowHandle pWindowHandle);
	virtual ~Swapchain();

	virtual void Present() = 0;

	RenderTarget* GetDefaultRenderTarget() const { return m_pDefaultRenderTarget; }

protected:
	uint m_Width;
	uint m_Height;
	WindowHandle m_pWindowHandle;

	RenderTarget* m_pDefaultRenderTarget;
};