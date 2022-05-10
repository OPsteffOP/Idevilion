// Folder: Graphics/Core/DX11

#pragma once
#include "Renderer.h"

#ifdef BP_PLATFORM_WINDOWS

class D3D11Renderer : public Renderer
{
	friend class D3D11Swapchain;
	friend class D3D11RenderTarget;
	friend class D3D11InputLayout;
	friend class D3D11Buffer;
	friend class D3D11PixelBuffer;
	friend class D3D11ShaderState;
	friend class D3D11Sampler;

public:
	D3D11Renderer();
	virtual ~D3D11Renderer() override;

	virtual void Initialize(RendererFlags flags) override;

	virtual void SetViewport(const Viewport& viewport) override;
	virtual void SetPrimitiveTopology(PrimitiveTopology topology) override;

	virtual void BindRenderTarget(RenderTarget* pRenderTarget) override;
	virtual void BindInputLayout(InputLayout* pInputLayout) override;
	virtual void BindIndexBuffer(Buffer* pBuffer) override;
	virtual void BindVertexBuffer(Buffer* pBuffer) override;
	virtual void BindVertexBuffers(const std::vector<Buffer*>& pBuffers) override;
	virtual void BindShaderState(ShaderState* pShaderState) override;

	virtual void BindConstantBufferVS(Buffer* pBuffer, uint index) override;
	virtual void BindConstantBuffersVS(const std::vector<Buffer*>& pBuffers, uint index) override;
	virtual void BindConstantBufferPS(Buffer* pBuffer, uint index) override;
	virtual void BindConstantBuffersPS(const std::vector<Buffer*>& pBuffers, uint index) override;
	virtual void BindConstantBufferCS(Buffer* pBuffer, uint index) override;
	virtual void BindConstantBuffersCS(const std::vector<Buffer*>& pBuffers, uint index) override;

	virtual void BindSamplerVS(Sampler* pSampler, uint index) override;
	virtual void BindSamplersVS(const std::vector<Sampler*>& pSamplers, uint index) override;
	virtual void BindSamplerPS(Sampler* pSampler, uint index) override;
	virtual void BindSamplersPS(const std::vector<Sampler*>& pSamplers, uint index) override;
	virtual void BindSamplerCS(Sampler* pSampler, uint index) override;
	virtual void BindSamplersCS(const std::vector<Sampler*>& pSamplers, uint index) override;

	virtual void BindReadBufferVS(Buffer* pBuffer, uint index) override;
	virtual void BindReadBufferVS(PixelBuffer* pBuffer, uint index) override;
	virtual void BindReadBuffersVS(const std::vector<Buffer*>& pBuffers, uint index) override;
	virtual void BindReadBuffersVS(const std::vector<PixelBuffer*>& pBuffers, uint index) override;
	virtual void BindReadBufferPS(Buffer* pBuffer, uint index) override;
	virtual void BindReadBufferPS(PixelBuffer* pBuffer, uint index) override;
	virtual void BindReadBuffersPS(const std::vector<Buffer*>& pBuffers, uint index) override;
	virtual void BindReadBuffersPS(const std::vector<PixelBuffer*>& pBuffers, uint index) override;
	virtual void BindReadBufferCS(Buffer* pBuffer, uint index) override;
	virtual void BindReadBufferCS(PixelBuffer* pBuffer, uint index) override;
	virtual void BindReadBuffersCS(const std::vector<Buffer*>& pBuffers, uint index) override;
	virtual void BindReadBuffersCS(const std::vector<PixelBuffer*>& pBuffers, uint index) override;

	virtual void Draw(uint vertexCount) override;
	virtual void DrawIndexed(uint indexCount) override;

	virtual void ClearRenderTarget(RenderTarget* pRenderTarget, const Color& pClearColor) override;

protected:
	virtual void BindConstantBuffer_Internal(ShaderType type, std::vector<Buffer*> pBuffers, uint index) override;
	// TODO: do the same code reuse with sampler and read buffers

private:
	unsigned int GetDeviceFlags(RendererFlags flags) const;

private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;

	// TODO: remove - this is just for temporary testing, this needs to be abstracted
	ID3D11DepthStencilState* m_pDepthStencilState;
	ID3D11BlendState* m_pBlendState;
	// TODO: remove - this is just for temporary testing, this needs to be abstracted
};

#endif