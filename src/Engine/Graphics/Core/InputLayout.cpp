// Folder: Graphics/Core

#include "EnginePCH.h"
#include "InputLayout.h"

#include "ShaderState.h"

InputLayout::InputLayout(ShaderState* pVertexShaderState, const std::vector<InputElement>& /*inputElements*/)
{
	if (pVertexShaderState->GetType() != ShaderType::VERTEX_SHADER)
	{
		LOG_ERROR("Input layout expects a vertex shader to compare input signature, but a different shader type is provided");
		return;
	}
}