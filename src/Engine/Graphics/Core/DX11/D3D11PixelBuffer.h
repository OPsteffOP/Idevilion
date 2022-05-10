// Folder: Graphics/Core/DX11

#pragma once
#include "PixelBuffer.h"

#ifdef BP_PLATFORM_WINDOWS

class D3D11PixelBuffer : public PixelBuffer
{
	friend class D3D11Renderer;
	friend class D3D11RenderTarget;
	friend class D3D11Swapchain;

public:
	D3D11PixelBuffer(uint width, uint height, PixelFormat format, BufferUsage usage, ShaderAccessFlags shaderAccess, CPUAccessFlags cpuAccess, BindFlags bindFlags);
	D3D11PixelBuffer(void* pData, uint width, uint height, PixelFormat format, BufferUsage usage, ShaderAccessFlags shaderAccess, CPUAccessFlags cpuAccess, BindFlags bindFlags);
	D3D11PixelBuffer(const std::string& imagePath, BufferUsage usage, ShaderAccessFlags shaderAccess, CPUAccessFlags cpuAccess, BindFlags bindFlags);

	virtual ~D3D11PixelBuffer() override;

	virtual void CopyFromPixelBuffer(PixelBuffer* pSrcBuffer) override;

protected:
	D3D11PixelBuffer(ID3D11Texture2D* pTexture, uint width, uint height, PixelFormat format, BufferUsage usage, ShaderAccessFlags shaderAccess, CPUAccessFlags cpuAccess, BindFlags bindFlags);
	
	void Initialize(void* pData, uint width, uint height, PixelFormat format, BufferUsage usage, ShaderAccessFlags shaderAccess, CPUAccessFlags cpuAccess, BindFlags bindFlags);
	virtual void LoadFromGPU(void* pData) override;

protected:
	ID3D11Texture2D* m_pTexture;
	ID3D11ShaderResourceView* m_pSRV;
	ID3D11UnorderedAccessView* m_pUAV;
};

#endif