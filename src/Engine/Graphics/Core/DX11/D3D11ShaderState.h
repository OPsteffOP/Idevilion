// Folder: Graphics/Core/DX11

#pragma once
#include "ShaderState.h"

#ifdef BP_PLATFORM_WINDOWS

class D3D11ShaderState : public ShaderState
{
	friend class D3D11Renderer;
	friend class D3D11InputLayout;

public:
	D3D11ShaderState(ShaderType type, const std::string& shaderPath, const std::vector<std::string>& defines = {});

	virtual ~D3D11ShaderState() override;

protected:
	ID3DBlob* m_pCompiledShader;

	union
	{
		ID3D11VertexShader* m_pVertexShader;
		ID3D11PixelShader* m_pPixelShader;
		ID3D11ComputeShader* m_pComputeShader;
	};
};

#endif