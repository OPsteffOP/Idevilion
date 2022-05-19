// Folder: Graphics/Core

#pragma once
#include "ShaderCompiler.h"

class ShaderState
{
public:
	ShaderState(ShaderType type, const std::string& shaderPath, const std::vector<std::string>& defines = {});

	virtual ~ShaderState() = default;

	ShaderType GetType() const { return m_Type; }

protected:
	const ShaderType m_Type;
	const std::string m_ShaderPath;
};