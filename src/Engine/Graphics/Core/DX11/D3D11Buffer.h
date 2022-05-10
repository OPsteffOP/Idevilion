// Folder: Graphics/Core/DX11

#pragma once
#include "Buffer.h"

#ifdef BP_PLATFORM_WINDOWS

// TODO: packing for constant buffers: https://docs.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-packing-ruless

class D3D11Buffer : public Buffer
{
	friend class D3D11Renderer;
	
public:
	D3D11Buffer(uint elementSize, uint elementCount, BufferUsage usage, ShaderAccessFlags shaderAccess, CPUAccessFlags cpuAccess, BindFlags bindFlags);
	D3D11Buffer(const void* pData, uint elementSize, uint elementCount, BufferUsage usage, ShaderAccessFlags shaderAccess, CPUAccessFlags cpuAccess, BindFlags bindFlags);

	virtual ~D3D11Buffer() override;

	virtual void UpdateData(const void* pData) override;
	virtual void UpdateData(const void* pData, uint size) override;

protected:
	bool Validate() const;

protected:
	ID3D11Buffer* m_pBuffer;
	ID3D11ShaderResourceView* m_pSRV;
	ID3D11UnorderedAccessView* m_pUAV;
};

#endif