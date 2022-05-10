// Folder: Graphics/Core/DX11

#pragma once
#include "InputLayout.h"

#ifdef BP_PLATFORM_WINDOWS

class D3D11InputLayout : public InputLayout
{
	friend class D3D11Renderer;
	
public:
	D3D11InputLayout(ShaderState* pVertexShaderState, const std::vector<InputElement>& inputElements);

	virtual ~D3D11InputLayout() override;

protected:
	ID3D11InputLayout* m_pInputLayout;
};

#endif