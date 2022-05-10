// Folder: Graphics/Core

#include "EnginePCH.h"
#include "ShaderState.h"

ShaderState::ShaderState(ShaderType type, const std::string& shaderPath, const std::vector<std::string>& /*defines*/)
	: m_Type(type)
	, m_ShaderPath(shaderPath)
{
	if (!std::filesystem::exists(shaderPath))
		LOG_ERROR("Shader path '%s' does not exist", shaderPath.c_str());
}