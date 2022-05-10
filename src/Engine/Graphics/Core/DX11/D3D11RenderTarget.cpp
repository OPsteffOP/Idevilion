// Folder: Graphics/Core/DX11

#include "EnginePCH.h"
#include "D3D11RenderTarget.h"

#ifdef BP_PLATFORM_WINDOWS

#include "D3D11Renderer.h"
#include "D3D11PixelBuffer.h"

D3D11RenderTarget::D3D11RenderTarget(PixelBuffer* pColorBuffer, PixelBuffer* pDepthStencilBuffer)
	: RenderTarget(pColorBuffer, pDepthStencilBuffer)
	, m_pRTV(nullptr)
	, m_pDSV(nullptr)
{
	HRESULT hr;

	D3D11Renderer* pRenderer = static_cast<D3D11Renderer*>(Renderer::GetRenderer());

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDescriptor{};
	renderTargetViewDescriptor.Format = ConvertPixelFormat(static_cast<D3D11PixelBuffer*>(m_pColorBuffer)->m_PixelFormat);
	renderTargetViewDescriptor.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDescriptor.Texture2D.MipSlice = 0;

	hr = pRenderer->m_pDevice->CreateRenderTargetView(static_cast<D3D11PixelBuffer*>(m_pColorBuffer)->m_pTexture, &renderTargetViewDescriptor, &m_pRTV);
	if (FAILED(hr))
	{
		LOG_ERROR("Failed to create RTV for D3D11 Texture2D");
		return;
	}

	if (pDepthStencilBuffer != nullptr)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDescriptor{};
		depthStencilViewDescriptor.Format = ConvertPixelFormat(static_cast<D3D11PixelBuffer*>(m_pDepthStencilBuffer)->m_PixelFormat);
		depthStencilViewDescriptor.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDescriptor.Texture2D.MipSlice = 0;

		hr = pRenderer->m_pDevice->CreateDepthStencilView(static_cast<D3D11PixelBuffer*>(m_pDepthStencilBuffer)->m_pTexture, &depthStencilViewDescriptor, &m_pDSV);
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create DSV for D3D11 Texture2D");
			return;
		}
	}
}

D3D11RenderTarget::~D3D11RenderTarget()
{
	SAFE_RELEASE(m_pRTV);
	SAFE_RELEASE(m_pDSV);
}

#endif