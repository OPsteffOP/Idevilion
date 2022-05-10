// Folder: Graphics/Core

#include "EnginePCH.h"
#include "Sampler.h"

Sampler::Sampler(SamplerFilter filter, RepeatMode repeatMode, const Color& borderColor)
	: Sampler(filter, repeatMode, 0, ComparisonOperation::ALWAYS_SUCCEED, borderColor)
{
	if (filter == SamplerFilter::ANISOTROPIC)
	{
		LOG_ERROR("Consider calling the Sampler constructor with anisotropyLevel when using anisotropic filtering");
		return;
	}
}

Sampler::Sampler(SamplerFilter filter, RepeatMode repeatMode, uint anisotropyLevel, const Color& borderColor)
	: Sampler(filter, repeatMode, anisotropyLevel, ComparisonOperation::ALWAYS_SUCCEED, borderColor)
{

}

Sampler::Sampler(SamplerFilter filter, RepeatMode repeatMode, ComparisonOperation comparison)
	: Sampler(filter, repeatMode, 0, comparison, Color{ 0.f, 0.f, 0.f, 1.f })
{

}

Sampler::Sampler(SamplerFilter filter, RepeatMode repeatMode, uint /*anisotropyLevel*/, ComparisonOperation comparison, const Color& /*borderColor*/)
	: m_Filter(filter)
	, m_RepeatMode(repeatMode)
	, m_ComparisonOperation(comparison)
{

}