// Folder: Graphics/Core

#pragma once

DEFINE_ENUM(PrimitiveTopology,
	TRIANGLES,
	LINES
);

DEFINE_ENUM(BufferUsage,
	DEFAULT,
	IMMUTABLE,
	DYNAMIC,
	STAGING
);

DEFINE_ENUM_FLAGS(ShaderAccessFlags,
	NO_ACCESS = 0,
	READ = 1 << 0,
	WRITE = 1 << 1
);

DEFINE_ENUM_FLAGS(CPUAccessFlags,
	NO_ACCESS = 0,
	READ = 1 << 0,
	WRITE = 1 << 1
);

DEFINE_ENUM_FLAGS(BindFlags,
	NONE = 0,
	VERTEX_BUFFER = 1 << 0,
	INDEX_BUFFER = 1 << 1,
	CONSTANT_BUFFER = 1 << 2,
	RENDER_TARGET = 1 << 3,
	DEPTH_STENCIL = 1 << 4,

	DRAWINDIRECT_ARGS = 1 << 5,
	STRUCTURED_BUFFER = 1 << 6
);

DEFINE_ENUM(ConstantBufferIndex,
	RESERVED_CAMERA_DATA = 0,
	USER_INDEX = 1
);

// https://www.3dgep.com/texturing-lighting-directx-11/#Texture_Sampler
DEFINE_ENUM(SamplerFilter,
	POINT,
	LINEAR,
	ANISOTROPIC
);

DEFINE_ENUM(RepeatMode,
	REPEAT_NORMAL,
	REPEAT_MIRORRED,
	CLAMP,
	BORDER_COLOR
);

DEFINE_ENUM(ComparisonOperation,
	ALWAYS_FAIL,
	ALWAYS_SUCCEED,
	SOURCE_EQUAL_TO_DESTINATION,
	SOURCE_NOT_EQUAL_TO_DESTINATION,
	SOURCE_LESS_THAN_DESTINATION,
	SOURCE_LESS_THAN_OR_EQUAL_TO_DESTINATION,
	SOURCE_GREATER_THAN_DESTINATION,
	SOURCE_GREATER_THAN_OR_EQUAL_TO_DESTINATION
);

DEFINE_ENUM(InputElementType,
	VERTEX_DATA,
	INSTANCE_DATA
);

enum class PixelFormat
{
#undef DEFINE_PIXEL_FORMAT
#define DEFINE_PIXEL_FORMAT(name, dxgi_format, components, bpp) name,
#include "PixelFormats.h"
#undef DEFINE_PIXEL_FORMAT
};

#ifdef BP_PLATFORM_WINDOWS
D3D11_PRIMITIVE_TOPOLOGY ConvertPrimitiveTopology(PrimitiveTopology topology);
DXGI_FORMAT ConvertPixelFormat(PixelFormat format);
D3D11_USAGE ConvertBufferUsage(BufferUsage usage);
D3D11_BIND_FLAG ConvertBindFlags(BindFlags flags, ShaderAccessFlags access);
D3D11_CPU_ACCESS_FLAG ConvertCPUAccess(CPUAccessFlags access);
D3D11_RESOURCE_MISC_FLAG ConvertMiscFlags(BindFlags flags);
D3D11_FILTER ConvertSamplerFilter(SamplerFilter filter);
D3D11_TEXTURE_ADDRESS_MODE ConvertRepeatMode(RepeatMode mode);
D3D11_COMPARISON_FUNC ConvertComparisonOperation(ComparisonOperation operation);
D3D11_INPUT_CLASSIFICATION ConvertInputElementType(InputElementType type);

DXGI_FORMAT GetDepthTextureFormat(PixelFormat format);
DXGI_FORMAT GetDepthShaderViewFormat(PixelFormat format);
#endif

uint GetComponents(PixelFormat format);
uint GetBPP(PixelFormat format);