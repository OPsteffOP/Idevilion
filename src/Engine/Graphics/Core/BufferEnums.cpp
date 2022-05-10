// Folder: Graphics/Core

#include "EnginePCH.h"
#include "BufferEnums.h"

#ifdef BP_PLATFORM_WINDOWS

D3D11_PRIMITIVE_TOPOLOGY ConvertPrimitiveTopology(PrimitiveTopology topology)
{
	switch (topology)
	{
	case PrimitiveTopology::TRIANGLES:
		return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	case PrimitiveTopology::LINES:
		return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	}

	LOG_ERROR("Unknown PrimitiveTopology specified");
	return D3D11_PRIMITIVE_TOPOLOGY(-1);
}

D3D11_USAGE ConvertBufferUsage(BufferUsage usage)
{
	switch (usage)
	{
	case BufferUsage::DEFAULT:
		return D3D11_USAGE_DEFAULT;
	case BufferUsage::IMMUTABLE:
		return D3D11_USAGE_IMMUTABLE;
	case BufferUsage::DYNAMIC:
		return D3D11_USAGE_DYNAMIC;
	case BufferUsage::STAGING:
		return D3D11_USAGE_STAGING;
	}

	LOG_ERROR("Specified BufferUsage can't be translated to a D3D11_USAGE");
	return D3D11_USAGE(0);
}

D3D11_BIND_FLAG ConvertBindFlags(BindFlags flags, ShaderAccessFlags access)
{
	uint bindFlags = 0;

	if (flags & BindFlags::VERTEX_BUFFER)
		bindFlags |= D3D11_BIND_VERTEX_BUFFER;
	if (flags & BindFlags::INDEX_BUFFER)
		bindFlags |= D3D11_BIND_INDEX_BUFFER;
	if (flags & BindFlags::CONSTANT_BUFFER)
		bindFlags |= D3D11_BIND_CONSTANT_BUFFER;
	if (flags & BindFlags::RENDER_TARGET)
		bindFlags |= D3D11_BIND_RENDER_TARGET;
	if (flags & BindFlags::DEPTH_STENCIL)
		bindFlags |= D3D11_BIND_DEPTH_STENCIL;

	if (access & ShaderAccessFlags::READ)
		bindFlags |= D3D11_BIND_SHADER_RESOURCE;
	if (access & ShaderAccessFlags::WRITE)
		bindFlags |= D3D11_BIND_UNORDERED_ACCESS;

	return D3D11_BIND_FLAG(bindFlags);
}

D3D11_CPU_ACCESS_FLAG ConvertCPUAccess(CPUAccessFlags access)
{
	uint bindFlags = 0;

	if (access & CPUAccessFlags::READ)
		bindFlags |= D3D11_CPU_ACCESS_READ;
	if (access & CPUAccessFlags::WRITE)
		bindFlags |= D3D11_CPU_ACCESS_WRITE;

	return D3D11_CPU_ACCESS_FLAG(bindFlags);
}

D3D11_RESOURCE_MISC_FLAG ConvertMiscFlags(BindFlags flags)
{
	uint miscFlags = 0;

	if (flags & BindFlags::DRAWINDIRECT_ARGS)
		miscFlags |= D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
	if (flags & BindFlags::STRUCTURED_BUFFER)
		miscFlags |= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	return D3D11_RESOURCE_MISC_FLAG(miscFlags);
}

D3D11_FILTER ConvertSamplerFilter(SamplerFilter filter)
{
	switch (filter)
	{
	case SamplerFilter::POINT:
		return D3D11_FILTER_MIN_MAG_MIP_POINT;
	case SamplerFilter::LINEAR:
		return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	case SamplerFilter::ANISOTROPIC:
		return D3D11_FILTER_ANISOTROPIC;
	}

	LOG_ERROR("Unknown sampler filter specified");
	return D3D11_FILTER(-1);
}

D3D11_TEXTURE_ADDRESS_MODE ConvertRepeatMode(RepeatMode mode)
{
	switch (mode)
	{
	case RepeatMode::REPEAT_NORMAL:
		return D3D11_TEXTURE_ADDRESS_WRAP;
	case RepeatMode::REPEAT_MIRORRED:
		return D3D11_TEXTURE_ADDRESS_MIRROR;
	case RepeatMode::CLAMP:
		return D3D11_TEXTURE_ADDRESS_CLAMP;
	case RepeatMode::BORDER_COLOR:
		return D3D11_TEXTURE_ADDRESS_BORDER;
	}

	LOG_ERROR("Unknown repeat mode specified");
	return D3D11_TEXTURE_ADDRESS_MODE(-1);
}

D3D11_COMPARISON_FUNC ConvertComparisonOperation(ComparisonOperation operation)
{
	switch (operation)
	{
	case ComparisonOperation::ALWAYS_FAIL:
		return D3D11_COMPARISON_NEVER;
	case ComparisonOperation::ALWAYS_SUCCEED:
		return D3D11_COMPARISON_ALWAYS;
	case ComparisonOperation::SOURCE_EQUAL_TO_DESTINATION:
		return D3D11_COMPARISON_EQUAL;
	case ComparisonOperation::SOURCE_NOT_EQUAL_TO_DESTINATION:
		return D3D11_COMPARISON_NOT_EQUAL;
	case ComparisonOperation::SOURCE_LESS_THAN_DESTINATION:
		return D3D11_COMPARISON_LESS;
	case ComparisonOperation::SOURCE_LESS_THAN_OR_EQUAL_TO_DESTINATION:
		return D3D11_COMPARISON_LESS_EQUAL;
	case ComparisonOperation::SOURCE_GREATER_THAN_DESTINATION:
		return D3D11_COMPARISON_GREATER;
	case ComparisonOperation::SOURCE_GREATER_THAN_OR_EQUAL_TO_DESTINATION:
		return D3D11_COMPARISON_GREATER_EQUAL;
	}

	LOG_ERROR("Unknown comparison operation specified");
	return D3D11_COMPARISON_FUNC(-1);
}

D3D11_INPUT_CLASSIFICATION ConvertInputElementType(InputElementType type)
{
	switch (type)
	{
	case InputElementType::VERTEX_DATA:
		return D3D11_INPUT_PER_VERTEX_DATA;
	case InputElementType::INSTANCE_DATA:
		return D3D11_INPUT_PER_INSTANCE_DATA;
	}

	LOG_ERROR("Unknown input element type");
	return D3D11_INPUT_CLASSIFICATION(-1);
}

DXGI_FORMAT ConvertPixelFormat(PixelFormat format)
{
#pragma warning(push)
#pragma warning(disable: 4060)
	switch (format)
	{
#define DEFINE_PIXEL_FORMAT(name, dxgi_format, components, bpp) case PixelFormat::name: return dxgi_format;
#include "PixelFormats.h"
#undef DEFINE_PIXEL_FORMAT
	}
#pragma warning(pop)

	LOG_ERROR("Specified PixelFormat can't be translated to a DXGI_FORMAT");
	return DXGI_FORMAT(0);
}

DXGI_FORMAT GetDepthTextureFormat(PixelFormat format)
{
	// TODO
	switch (format)
	{
	//case PixelFormat::D16_UNORM:
	//	return DXGI_FORMAT::DXGI_FORMAT_R16_TYPELESS;
	case PixelFormat::D24_UNORM_S8_UINT:
		return DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS;
	//case PixelFormat::D32_FLOAT:
	//	return DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
	//case PixelFormat::D32_FLOAT_S8X24_UINT:
	//	return DXGI_FORMAT::DXGI_FORMAT_R32G8X24_TYPELESS;
	}

	return DXGI_FORMAT_UNKNOWN;
}

DXGI_FORMAT GetDepthShaderViewFormat(PixelFormat format)
{
	// TODO
	switch (format)
	{
	//case PixelFormat::D16_UNORM:
	//	return DXGI_FORMAT::DXGI_FORMAT_R16_FLOAT;
	case PixelFormat::D24_UNORM_S8_UINT:
		return DXGI_FORMAT::DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	//case PixelFormat::D32_FLOAT:
	//	return DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
	//case PixelFormat::D32_FLOAT_S8X24_UINT:
	//	return DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
	}

	return DXGI_FORMAT_UNKNOWN;
}

#endif

uint GetComponents(PixelFormat format)
{
#undef DEFINE_PIXEL_FORMAT

#pragma warning(push)
#pragma warning(disable: 4060)
	switch (format)
	{
#define DEFINE_PIXEL_FORMAT(name, dxgi_format, components, bpp) case PixelFormat::name: return components;
#include "PixelFormats.h"
#undef DEFINE_PIXEL_FORMAT
	}
#pragma warning(pop)

	LOG_ERROR("Unknown PixelFormat specified");
	return 0;
}

uint GetBPP(PixelFormat format)
{
#undef DEFINE_PIXEL_FORMAT

#pragma warning(push)
#pragma warning(disable: 4060)
	switch (format)
	{
#define DEFINE_PIXEL_FORMAT(name, dxgi_format, components, bpp) case PixelFormat::name: return bpp;
#include "PixelFormats.h"
#undef DEFINE_PIXEL_FORMAT
	}
#pragma warning(pop)

	LOG_ERROR("Unknown PixelFormat specified");
	return 0;
}