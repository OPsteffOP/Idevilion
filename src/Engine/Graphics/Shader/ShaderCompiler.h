// Folder: Graphics/Shader

#pragma once
#include "CompiledShaderData.h"

#ifdef DEV_BUILD

class ShaderCompiler
{
public:
	static ShaderCompiler* GetInstance();

	CompiledShaderData CompileShader(const std::string& shaderPath, const std::vector<std::string>& defines, ShaderType type, ShaderBackend backend);
	CompiledShaderData CompileShaderNoCache(const std::string& shaderPath, const std::vector<std::string>& defines, ShaderType type, ShaderBackend backend);

private:
	void CompileShader_DirectX(const std::string& hlslCode, const std::vector<std::string>& defines, ShaderType type, CompiledShaderData& data) const;
};

#endif