// Folder: Graphics/Core/DX11

#include "EnginePCH.h"
#include "D3D11ShaderState.h"

#ifdef BP_PLATFORM_WINDOWS

#include "D3D11Renderer.h"

D3D11ShaderState::D3D11ShaderState(ShaderType type, const std::string& shaderPath, const std::vector<std::string>& defines)
	: ShaderState(type, shaderPath)
	, m_CompiledShaderData()
	, m_pVertexShader(nullptr)
{
#ifdef DEV_BUILD
	m_CompiledShaderData = ShaderCompiler::GetInstance()->CompileShader(shaderPath, defines, type, ShaderBackend::DIRECTX_11);
#else
	// TODO: load the compiled shader here
#endif

	if (m_CompiledShaderData.pBuffer == nullptr)
	{
		LOG_ERROR("Failed to compile/load shader at path=%s", shaderPath.c_str());
		return;
	}

	HRESULT hr = E_FAIL;
	D3D11Renderer* pRenderer = static_cast<D3D11Renderer*>(Renderer::GetRenderer());

	switch (m_Type)
	{
	case ShaderType::VERTEX_SHADER:
		hr = pRenderer->m_pDevice->CreateVertexShader(m_CompiledShaderData.pBuffer, m_CompiledShaderData.size, NULL, &m_pVertexShader);
		break;
	case ShaderType::PIXEL_SHADER:
		hr = pRenderer->m_pDevice->CreatePixelShader(m_CompiledShaderData.pBuffer, m_CompiledShaderData.size, NULL, &m_pPixelShader);
		break;
	case ShaderType::COMPUTE_SHADER:
		hr = pRenderer->m_pDevice->CreateComputeShader(m_CompiledShaderData.pBuffer, m_CompiledShaderData.size, NULL, &m_pComputeShader);
		break;
	}

	if (FAILED(hr))
	{
		const char* typesStr[(int)ShaderType::_COUNT];
		typesStr[(int)ShaderType::VERTEX_SHADER] = "VERTEX_SHADER";
		typesStr[(int)ShaderType::PIXEL_SHADER] = "PIXEL_SHADER";
		typesStr[(int)ShaderType::COMPUTE_SHADER] = "COMPUTE_SHADER";

		LOG_ERROR("Failed to create shader of type '%s' with path: %s", typesStr[(int)type], shaderPath.c_str());
		return;
	}
}

D3D11ShaderState::~D3D11ShaderState()
{
	switch (m_Type)
	{
	case ShaderType::VERTEX_SHADER:
		SAFE_RELEASE(m_pVertexShader);
		break;
	case ShaderType::PIXEL_SHADER:
		SAFE_RELEASE(m_pPixelShader);
		break;
	case ShaderType::COMPUTE_SHADER:
		SAFE_RELEASE(m_pComputeShader);
		break;
	}
}

#endif