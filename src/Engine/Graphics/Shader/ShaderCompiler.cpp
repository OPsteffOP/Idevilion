// Folder: Graphics/Shader

#include "EnginePCH.h"
#include "ShaderCompiler.h"

#ifdef DEV_BUILD

#include "ShaderCache.h"

#include <sstream>

ShaderCompiler* ShaderCompiler::GetInstance()
{
	static ShaderCompiler* pInstance = nullptr;
	if (pInstance == nullptr)
	{
		ALLOW_NEXT_ALLOCATION_LEAK();
		pInstance = new ShaderCompiler();
	}

	return pInstance;
}

CompiledShaderData ShaderCompiler::CompileShader(const std::string& shaderPath, const std::vector<std::string>& defines, ShaderType type, ShaderBackend backend)
{
	if (CommandLine::IsFlagSet("--no-shader-cache"))
		return CompileShaderNoCache(shaderPath, defines, type, backend);

	CompiledShaderData data;

	if (!CommandLine::IsFlagSet("--force-recompile-shaders"))
		data = ShaderCache::GetInstance()->LoadShader(shaderPath, defines, backend);

	if (data.pBuffer == nullptr)
	{
		data = CompileShaderNoCache(shaderPath, defines, type, backend);

		if (data.pBuffer != nullptr)
			ShaderCache::GetInstance()->StoreShader(data, shaderPath, defines, backend);
	}

	return data;
}

CompiledShaderData ShaderCompiler::CompileShaderNoCache(const std::string& shaderPath, const std::vector<std::string>& defines, ShaderType type, ShaderBackend backend)
{
	LOG_DEBUG("Compiling shader: %s", shaderPath.c_str());

	CompiledShaderData data;

	std::string hlslCode;
	{
		std::ifstream input(shaderPath);
		std::ostringstream fileStringStream;
		fileStringStream << input.rdbuf();
		hlslCode = fileStringStream.str();
	}

	switch (backend)
	{
	case ShaderBackend::DIRECTX_11:
		CompileShader_DirectX(hlslCode, defines, type, data);
		break;
	}

	return data;
}

void ShaderCompiler::CompileShader_DirectX(const std::string& hlslCode, const std::vector<std::string>& defines, ShaderType type, CompiledShaderData& data) const
{
	std::vector<D3D_SHADER_MACRO> shaderMacros;
	shaderMacros.reserve(defines.size() + 1);
	for (const std::string& define : defines)
	{
		D3D_SHADER_MACRO& macro = shaderMacros.emplace_back();
		macro.Name = define.c_str();
		macro.Definition = "";
	}
	shaderMacros.push_back(D3D_SHADER_MACRO{ NULL, NULL });

	const char* targets[(int)ShaderType::_COUNT];
	targets[(int)ShaderType::VERTEX_SHADER] = "vs_5_0";
	targets[(int)ShaderType::PIXEL_SHADER] = "ps_5_0";
	targets[(int)ShaderType::COMPUTE_SHADER] = "cs_5_0";

	UINT flags = 0;
	if (CommandLine::IsFlagSet("--shader-debug"))
	{
		flags |= D3DCOMPILE_DEBUG;
		flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
	}

	ID3DBlob* pCode;
	ID3DBlob* pError;
	HRESULT hr = D3DCompile(hlslCode.c_str(), hlslCode.size(), NULL, shaderMacros.data(), D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", targets[(int)type], flags, 0, &pCode, &pError);

	if (FAILED(hr))
	{
		const char* typesStr[(int)ShaderType::_COUNT];
		typesStr[(int)ShaderType::VERTEX_SHADER] = "VERTEX_SHADER";
		typesStr[(int)ShaderType::PIXEL_SHADER] = "PIXEL_SHADER";
		typesStr[(int)ShaderType::COMPUTE_SHADER] = "COMPUTE_SHADER";

		const std::string errorMessage = std::string((const char*)pError->GetBufferPointer(), pError->GetBufferSize());

		LOG_ERROR("Failed to compile shader of type '%s'", typesStr[(int)type]);
		LOG_ERROR("%s", errorMessage.c_str());

		SAFE_RELEASE(pError);

		data.pBuffer = nullptr;
		data.size = 0;
		return;
	}

	data.pBuffer = new char[pCode->GetBufferSize()];
	memcpy(data.pBuffer, pCode->GetBufferPointer(), pCode->GetBufferSize());
	data.size = pCode->GetBufferSize();

	SAFE_RELEASE(pCode);
}

#endif