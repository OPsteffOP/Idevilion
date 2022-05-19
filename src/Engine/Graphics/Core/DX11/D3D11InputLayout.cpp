// Folder: Graphics/Core/DX11

#include "EnginePCH.h"
#include "D3D11InputLayout.h"

#ifdef BP_PLATFORM_WINDOWS

#include "D3D11Renderer.h"
#include "D3D11ShaderState.h"

D3D11InputLayout::D3D11InputLayout(ShaderState* pVertexShaderState, const std::vector<InputElement>& inputElements)
	: InputLayout(pVertexShaderState, inputElements)
	, m_pInputLayout(nullptr)
{
	HRESULT hr;

	D3D11Renderer* pRenderer = static_cast<D3D11Renderer*>(Renderer::GetRenderer());

	std::vector<D3D11_INPUT_ELEMENT_DESC> elements;
	elements.reserve(inputElements.size());

	for (const InputElement& inputElement : inputElements)
	{
		D3D11_INPUT_ELEMENT_DESC& element = elements.emplace_back();
		element.SemanticName = inputElement.semanticName.c_str();
		element.SemanticIndex = inputElement.semanticIndex;
		element.Format = ConvertPixelFormat(inputElement.format);
		element.InputSlot = 0;
		element.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		element.InputSlotClass = ConvertInputElementType(inputElement.type);
		element.InstanceDataStepRate = (inputElement.type == InputElementType::INSTANCE_DATA) ? 1 : 0;
	}

	D3D11ShaderState* pD3D11ShaderState = static_cast<D3D11ShaderState*>(pVertexShaderState);

	hr = pRenderer->m_pDevice->CreateInputLayout(elements.data(), (UINT)elements.size(), pD3D11ShaderState->m_CompiledShaderData.pBuffer,
		pD3D11ShaderState->m_CompiledShaderData.size, &m_pInputLayout);
	if (FAILED(hr))
	{
		LOG_ERROR("Failed to create D3D11 input layout");
		return;
	}
}

D3D11InputLayout::~D3D11InputLayout()
{
	SAFE_RELEASE(m_pInputLayout);
}

#endif