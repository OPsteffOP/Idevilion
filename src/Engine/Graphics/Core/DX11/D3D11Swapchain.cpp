// Folder: Graphics/Core/DX11

#include "EnginePCH.h"
#include "D3D11Swapchain.h"

#ifdef BP_PLATFORM_WINDOWS

#include "D3D11Renderer.h"
#include "D3D11RenderTarget.h"
#include "D3D11PixelBuffer.h"

D3D11Swapchain::D3D11Swapchain(uint width, uint height, WindowHandle windowHandle)
	: Swapchain(width, height, windowHandle)
	, m_pSwapchain(nullptr)
{
	HRESULT hr;

	D3D11Renderer* pRenderer = static_cast<D3D11Renderer*>(Renderer::GetRenderer());

	PixelFormat colorPixelFormat = PixelFormat::R8G8B8A8_UNORM;
	PixelFormat depthPixelFormat = PixelFormat::D24_UNORM_S8_UINT;

	// TODO: create device for best graphics card (iterate over devices)
	/*
		result = CreateDXGIFactory2(0, IID_PPV_ARGS(&m_pDXGIFactory));
	AE_VALIDATE_HRESULT_AND_RETURN(result, "Failed to create DXGI factory");

	IDXGIAdapter1* pAdapter = nullptr;
	IDXGIAdapter1* pTestAdapter;
	for (uint adapterIndex = 0; m_pDXGIFactory->EnumAdapters1(adapterIndex, &pTestAdapter) != DXGI_ERROR_NOT_FOUND; ++adapterIndex)
	{
		DXGI_ADAPTER_DESC1 testAdapterDesc;
		pTestAdapter->GetDesc1(&testAdapterDesc);

		if (testAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			continue;

		if (SUCCEEDED(D3D11CreateDevice(pTestAdapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, deviceFlags, featureLevels,
			sizeof(featureLevels) / sizeof(D3D_FEATURE_LEVEL), D3D11_SDK_VERSION, NULL, NULL, NULL)))
		{
			// making sure the adapter is capable of creating a device with the requirements
			if (pAdapter == nullptr)
			{
				pAdapter = pTestAdapter;
				continue;
			}

			DXGI_ADAPTER_DESC1 adapterDesc;
			pAdapter->GetDesc1(&adapterDesc);

			if (testAdapterDesc.DedicatedVideoMemory > adapterDesc.DedicatedVideoMemory)
				pAdapter = pTestAdapter; // choosing the adapter with the most vram
		}
	}
	*/

	DXGI_SWAP_CHAIN_DESC1 swapchainDescriptor{};
	swapchainDescriptor.Width = width; // resolution width
	swapchainDescriptor.Height = height;  // resolution height
	swapchainDescriptor.Format = ConvertPixelFormat(colorPixelFormat);
	swapchainDescriptor.Stereo = FALSE;
	swapchainDescriptor.SampleDesc.Count = 1;
	swapchainDescriptor.SampleDesc.Quality = 0;
	swapchainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
	swapchainDescriptor.BufferCount = 2;
	swapchainDescriptor.Scaling = DXGI_SCALING_NONE;
	swapchainDescriptor.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapchainDescriptor.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapchainDescriptor.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	IDXGIDevice2* pDxgiDevice;
	IDXGIAdapter2* pDxgiAdapter;
	IDXGIFactory2* pDxgiFactory;

	hr = pRenderer->m_pDevice->QueryInterface(__uuidof(IDXGIDevice2), reinterpret_cast<void**>(&pDxgiDevice));
	if (FAILED(hr))
	{
		LOG_ERROR("Failed to get the IDXGIDevice from D3D11 device!");
		return;
	}

	hr = pDxgiDevice->GetParent(__uuidof(IDXGIAdapter2), reinterpret_cast<void**>(&pDxgiAdapter));
	if (FAILED(hr))
	{
		LOG_ERROR("Failed to get the IDXGIAdapter from IDXGIDevice!");
		return;
	}

	hr = pDxgiAdapter->GetParent(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&pDxgiFactory));
	if (FAILED(hr))
	{
		LOG_ERROR("Failed to get the IDXGIFactory from IDXGIAdapter!");
		return;
	}

	hr = pDxgiFactory->CreateSwapChainForHwnd(pRenderer->m_pDevice, (HWND)m_pWindowHandle, &swapchainDescriptor, NULL, NULL, &m_pSwapchain);
	if (FAILED(hr))
	{
		LOG_ERROR("Failed to create IDXGISwapchain");
		return;
	}

	pDxgiDevice->Release();
	pDxgiAdapter->Release();
	pDxgiFactory->Release();

	ID3D11Texture2D* pBackBuffer;
	hr = m_pSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
	if (FAILED(hr))
	{
		LOG_ERROR("Failed to get the back buffer from the IDXGISwapchain");
		return;
	}

	D3D11PixelBuffer* pColorPixelBuffer = new D3D11PixelBuffer(pBackBuffer, width, height, colorPixelFormat, BufferUsage::DEFAULT, ShaderAccessFlags::READ, CPUAccessFlags::NO_ACCESS, BindFlags::RENDER_TARGET);
	D3D11PixelBuffer* pDepthStencilBuffer = new D3D11PixelBuffer(width, height, depthPixelFormat, BufferUsage::DEFAULT, ShaderAccessFlags::READ, CPUAccessFlags::NO_ACCESS, BindFlags::DEPTH_STENCIL);

	m_pDefaultRenderTarget = new D3D11RenderTarget(pColorPixelBuffer, pDepthStencilBuffer);
}

D3D11Swapchain::~D3D11Swapchain()
{
	SAFE_RELEASE(m_pSwapchain);
}

void D3D11Swapchain::Present()
{
	m_pSwapchain->Present(0, NULL);
	Renderer::GetRenderer()->BindRenderTarget(m_pDefaultRenderTarget);
}

#endif