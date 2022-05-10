// Folder: Graphics/Core

#pragma once
#include "BufferEnums.h"

class ShaderState;

struct InputElement
{
	std::string semanticName;
	uint semanticIndex;
	PixelFormat format;
	InputElementType type;
};

class InputLayout
{
public:
	InputLayout(ShaderState* pVertexShaderState, const std::vector<InputElement>& inputElements);

	virtual ~InputLayout() = default;
};