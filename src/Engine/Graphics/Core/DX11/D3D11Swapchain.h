// Folder: Graphics/Core/DX11

#pragma once
#include "Swapchain.h"

#ifdef BP_PLATFORM_WINDOWS

class D3D11RenderTarget;

class D3D11Swapchain : public Swapchain
{
	friend class D3D11Renderer;

public:
	D3D11Swapchain(uint width, uint height, WindowHandle windowHandle);

	virtual ~D3D11Swapchain() override;

	virtual void Present() override;

protected:
	IDXGISwapChain1* m_pSwapchain;
};

#endif