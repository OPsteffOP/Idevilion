// Folder: Graphics/Shader

#pragma once

#include "CompiledShaderData.h"

class ShaderCache
{
public:
	static ShaderCache* GetInstance();

	CompiledShaderData LoadShader(const std::string& shaderPath, const std::vector<std::string>& defines, ShaderBackend backend);
	void StoreShader(const CompiledShaderData& data, const std::string& shaderPath, const std::vector<std::string>& defines, ShaderBackend backend);

private:
	std::string GetCompiledShaderPath(const std::string& shaderPath, const std::vector<std::string>& defines, ShaderBackend backend);
};