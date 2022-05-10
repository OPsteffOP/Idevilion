// Folder: Graphics/Core/DX11

#include "EnginePCH.h"
#include "D3D11Sampler.h"

#ifdef BP_PLATFORM_WINDOWS

#include "D3D11Renderer.h"

D3D11Sampler::D3D11Sampler(SamplerFilter filter, RepeatMode repeatMode, const Color& borderColor)
	: D3D11Sampler(filter, repeatMode, 0, ComparisonOperation::ALWAYS_SUCCEED, borderColor)
{

}

D3D11Sampler::D3D11Sampler(SamplerFilter filter, RepeatMode repeatMode, uint anisotropyLevel, const Color& borderColor)
	: D3D11Sampler(filter, repeatMode, anisotropyLevel, ComparisonOperation::ALWAYS_SUCCEED, borderColor)
{

}

D3D11Sampler::D3D11Sampler(SamplerFilter filter, RepeatMode repeatMode, ComparisonOperation comparison)
	: D3D11Sampler(filter, repeatMode, 0, comparison, Color{ 0.f, 0.f, 0.f, 1.f })
{

}

D3D11Sampler::D3D11Sampler(SamplerFilter filter, RepeatMode repeatMode, uint anisotropyLevel, ComparisonOperation comparison, const Color& borderColor)
	: Sampler(filter, repeatMode, anisotropyLevel, comparison, borderColor)
{
	HRESULT hr;

	D3D11Renderer* pRenderer = static_cast<D3D11Renderer*>(Renderer::GetRenderer());

	D3D11_SAMPLER_DESC samplerDescriptor{};
	samplerDescriptor.Filter = ConvertSamplerFilter(filter);
	samplerDescriptor.AddressU = ConvertRepeatMode(repeatMode);
	samplerDescriptor.AddressV = ConvertRepeatMode(repeatMode);
	samplerDescriptor.AddressW = ConvertRepeatMode(repeatMode);
	samplerDescriptor.MipLODBias = 0;
	samplerDescriptor.MaxAnisotropy = anisotropyLevel;
	samplerDescriptor.ComparisonFunc = ConvertComparisonOperation(comparison);
	samplerDescriptor.BorderColor[0] = borderColor.r;
	samplerDescriptor.BorderColor[1] = borderColor.g;
	samplerDescriptor.BorderColor[2] = borderColor.b;
	samplerDescriptor.BorderColor[3] = borderColor.a;
	samplerDescriptor.MinLOD = 0;
	samplerDescriptor.MaxLOD = 0;

	hr = pRenderer->m_pDevice->CreateSamplerState(&samplerDescriptor, &m_pSamplerState);
	if (FAILED(hr))
	{
		LOG_ERROR("Failed to create D3D11 sampler state");
		return;
	}
}

D3D11Sampler::~D3D11Sampler()
{
	SAFE_RELEASE(m_pSamplerState);
}

#endif