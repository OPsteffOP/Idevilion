// Folder: Graphics/Core

#pragma once
#include "BufferEnums.h"

class Sampler
{
public:
	Sampler(SamplerFilter filter, RepeatMode repeatMode, const Color& borderColor = { 0.f, 0.f, 0.f, 1.f });
	Sampler(SamplerFilter filter, RepeatMode repeatMode, uint anisotropyLevel, const Color& borderColor = { 0.f, 0.f, 0.f, 1.f });
	Sampler(SamplerFilter filter, RepeatMode repeatMode, ComparisonOperation comparison);

	virtual ~Sampler() = default;

protected:
	Sampler(SamplerFilter filter, RepeatMode repeatMode, uint anisotropyLevel, ComparisonOperation comparison, const Color& borderColor);

protected:
	const SamplerFilter m_Filter;
	const RepeatMode m_RepeatMode;
	const ComparisonOperation m_ComparisonOperation;
};