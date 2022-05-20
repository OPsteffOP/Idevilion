// Folder: Graphics/Shader

#pragma once

DEFINE_ENUM(ShaderBackend,
	DIRECTX_11
);

namespace ShaderBackendHelper
{
	inline std::string ToString(ShaderBackend backend)
	{
		switch (backend)
		{
		case ShaderBackend::DIRECTX_11:
			return "DX11";
		}

		DEBUG_ASSERT(false, "Forgot to add one of the enum values to the ToString() function?");
		return "UNKNOWN";
	}
}

DEFINE_ENUM(ShaderType,
	VERTEX_SHADER,
	PIXEL_SHADER,
	COMPUTE_SHADER
);

struct CompiledShaderData
{
	char* pBuffer;
	size_t size;

	CompiledShaderData()
		: pBuffer(nullptr)
		, size(0)
	{}

	~CompiledShaderData()
	{
		SAFE_DELETE_ARRAY(pBuffer);
	}

	CompiledShaderData(const CompiledShaderData& data) = delete;
	CompiledShaderData(CompiledShaderData&& data) noexcept
		: pBuffer(data.pBuffer)
		, size(data.size)
	{
		data.pBuffer = nullptr;
		data.size = 0;
	}

	CompiledShaderData& operator=(const CompiledShaderData& data) = delete;
	CompiledShaderData& operator=(CompiledShaderData&& data) noexcept
	{
		pBuffer = data.pBuffer;
		size = data.size;

		data.pBuffer = nullptr;
		data.size = 0;

		return *this;
	}
};