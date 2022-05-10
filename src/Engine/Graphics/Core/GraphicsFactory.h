// Folder: Graphics/Core

#pragma once

#include "Renderer.h"

#include "D3D11Renderer.h"
#include "D3D11Swapchain.h"
#include "D3D11RenderTarget.h"
#include "D3D11InputLayout.h"
#include "D3D11Buffer.h"
#include "D3D11PixelBuffer.h"
#include "D3D11ShaderState.h"
#include "D3D11Sampler.h"

#ifdef BP_PLATFORM_ARM64
// TODO
#endif

namespace GraphicsFactory
{
	inline Renderer* CreateRenderer(RenderAPI preferredAPI)
	{
		ALLOW_NEXT_ALLOCATION_LEAK();

		// Try to use the preferred render API (only if it's available for the current platform)
		switch (preferredAPI)
		{
#ifdef BP_PLATFORM_WINDOWS
		case RenderAPI::DirectX11:
			return new D3D11Renderer();
#endif
		}

		// Preferred render API can't be used on current platform, use default for that platform instead
#ifdef BP_PLATFORM_WINDOWS
		return new D3D11Renderer();
#endif
	}

	template<typename... Args>
	Swapchain* CreateSwapchain(Args&&... args)
	{
		Renderer* pRenderer = Renderer::GetRenderer();

		switch (pRenderer->GetCurrentAPI())
		{
#ifdef BP_PLATFORM_WINDOWS
		case RenderAPI::DirectX11:
			return new D3D11Swapchain(std::forward<Args>(args)...);
#endif
		}

		LOG_ERROR("CreateSwapchain for current RenderAPI not set");
		return nullptr;
	}

	template<typename... Args>
	RenderTarget* CreateRenderTarget(Args&&... args)
	{
		Renderer* pRenderer = Renderer::GetRenderer();

		switch (pRenderer->GetCurrentAPI())
		{
#ifdef BP_PLATFORM_WINDOWS
		case RenderAPI::DirectX11:
			return new D3D11RenderTarget(std::forward<Args>(args)...);
#endif
		}

		LOG_ERROR("CreateRenderTarget for current RenderAPI not set");
		return nullptr;
	}

	template<typename... Args>
	InputLayout* CreateInputLayout(Args&&... args)
	{
		Renderer* pRenderer = Renderer::GetRenderer();

		switch (pRenderer->GetCurrentAPI())
		{
#ifdef BP_PLATFORM_WINDOWS
		case RenderAPI::DirectX11:
			return new D3D11InputLayout(std::forward<Args>(args)...);
#endif
		}

		LOG_ERROR("CreateInputLayout for current RenderAPI not set");
		return nullptr;
	}

	template<typename... Args>
	Buffer* CreateBuffer(Args&&... args)
	{
		Renderer* pRenderer = Renderer::GetRenderer();

		switch (pRenderer->GetCurrentAPI())
		{
#ifdef BP_PLATFORM_WINDOWS
		case RenderAPI::DirectX11:
			return new D3D11Buffer(std::forward<Args>(args)...);
#endif
		}

		LOG_ERROR("CreateBuffer for current RenderAPI not set");
		return nullptr;
	}

	template<typename... Args>
	PixelBuffer* CreatePixelBuffer(Args&&... args)
	{
		Renderer* pRenderer = Renderer::GetRenderer();

		switch (pRenderer->GetCurrentAPI())
		{
#ifdef BP_PLATFORM_WINDOWS
		case RenderAPI::DirectX11:
			return new D3D11PixelBuffer(std::forward<Args>(args)...);
#endif
		}

		LOG_ERROR("CreatePixelBuffer for current RenderAPI not set");
		return nullptr;
	}

	template<typename... Args>
	ShaderState* CreateShaderState(Args&&... args)
	{
		Renderer* pRenderer = Renderer::GetRenderer();

		switch (pRenderer->GetCurrentAPI())
		{
#ifdef BP_PLATFORM_WINDOWS
		case RenderAPI::DirectX11:
			return new D3D11ShaderState(std::forward<Args>(args)...);
#endif
		}

		LOG_ERROR("CreateShaderState for current RenderAPI not set");
		return nullptr;
	}

	template<typename... Args>
	Sampler* CreateSampler(Args&&... args)
	{
		Renderer* pRenderer = Renderer::GetRenderer();

		switch (pRenderer->GetCurrentAPI())
		{
#ifdef BP_PLATFORM_WINDOWS
		case RenderAPI::DirectX11:
			return new D3D11Sampler(std::forward<Args>(args)...);
#endif
		}

		LOG_ERROR("CreateSampler for current RenderAPI not set");
		return nullptr;
	}
}