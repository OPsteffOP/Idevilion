// Folder: Graphics/Core/DX11

#pragma once
#include "RenderTarget.h"

#ifdef BP_PLATFORM_WINDOWS

class D3D11RenderTarget : public RenderTarget
{
	friend class D3D11Renderer;
	friend class D3D11Swapchain;

public:
	D3D11RenderTarget(PixelBuffer* pColorBuffer, PixelBuffer* pDepthStencilBuffer);

	virtual ~D3D11RenderTarget() override;

protected:
	ID3D11RenderTargetView* m_pRTV;
	ID3D11DepthStencilView* m_pDSV;
};

#endif