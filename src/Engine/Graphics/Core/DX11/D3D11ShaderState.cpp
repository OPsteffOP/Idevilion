// Folder: Graphics/Core/DX11

#include "EnginePCH.h"
#include "D3D11ShaderState.h"

#ifdef BP_PLATFORM_WINDOWS

#include "D3D11Renderer.h"

D3D11ShaderState::D3D11ShaderState(ShaderType type, const std::string& shaderPath, const std::vector<std::string>& defines)
	: ShaderState(type, shaderPath)
	, m_pCompiledShader(nullptr)
	, m_pVertexShader(nullptr)
{
	HRESULT hr;

	D3D11Renderer* pRenderer = static_cast<D3D11Renderer*>(Renderer::GetRenderer());

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
	// TODO: check whether the debug flags should be set, instead of always setting them
	flags |= D3DCOMPILE_DEBUG;
	flags |= D3DCOMPILE_SKIP_OPTIMIZATION;

	ID3DBlob* pError;
	hr = D3DCompileFromFile(Utils::StringToWideString(m_ShaderPath).c_str(), shaderMacros.data(), D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main", targets[(int)type], flags, 0, &m_pCompiledShader, &pError);
	if (FAILED(hr))
	{
		const char* typesStr[(int)ShaderType::_COUNT];
		typesStr[(int)ShaderType::VERTEX_SHADER] = "VERTEX_SHADER";
		typesStr[(int)ShaderType::PIXEL_SHADER] = "PIXEL_SHADER";
		typesStr[(int)ShaderType::COMPUTE_SHADER] = "COMPUTE_SHADER";

		LOG_ERROR("Failed to compile shader of type '%s' with path: %s", typesStr[(int)type], shaderPath.c_str());
		LOG_ERROR("%s", std::string((const char*)pError->GetBufferPointer(), pError->GetBufferSize()).c_str());

		SAFE_RELEASE(pError);
		return;
	}

	switch (m_Type)
	{
	case ShaderType::VERTEX_SHADER:
		hr = pRenderer->m_pDevice->CreateVertexShader(m_pCompiledShader->GetBufferPointer(), m_pCompiledShader->GetBufferSize(), NULL, &m_pVertexShader);
		break;
	case ShaderType::PIXEL_SHADER:
		hr = pRenderer->m_pDevice->CreatePixelShader(m_pCompiledShader->GetBufferPointer(), m_pCompiledShader->GetBufferSize(), NULL, &m_pPixelShader);
		break;
	case ShaderType::COMPUTE_SHADER:
		hr = pRenderer->m_pDevice->CreateComputeShader(m_pCompiledShader->GetBufferPointer(), m_pCompiledShader->GetBufferSize(), NULL, &m_pComputeShader);
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

	SAFE_RELEASE(m_pCompiledShader);
}

#endif