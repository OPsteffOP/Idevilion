// Folder: Graphics/Shader

#pragma once
#ifdef DEV_BUILD

#include "CompiledShaderData.h"

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