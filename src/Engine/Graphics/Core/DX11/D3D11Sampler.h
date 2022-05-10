// Folder: Graphics/Core/DX11

#pragma once
#include "Sampler.h"

#ifdef BP_PLATFORM_WINDOWS

class D3D11Sampler : public Sampler
{
	friend class D3D11Renderer;

public:
	D3D11Sampler(SamplerFilter filter, RepeatMode repeatMode, const Color& borderColor = { 0.f, 0.f, 0.f, 1.f });
	D3D11Sampler(SamplerFilter filter, RepeatMode repeatMode, uint anisotropyLevel, const Color& borderColor = { 0.f, 0.f, 0.f, 1.f });
	D3D11Sampler(SamplerFilter filter, RepeatMode repeatMode, ComparisonOperation comparison);

	virtual ~D3D11Sampler() override;

protected:
	D3D11Sampler(SamplerFilter filter, RepeatMode repeatMode, uint anisotropyLevel, ComparisonOperation comparison, const Color& borderColor);

protected:
	ID3D11SamplerState* m_pSamplerState;
};

#endif