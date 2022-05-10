// Folder: Graphics/Core

#pragma once
#include "BufferEnums.h"
#include "Viewport.h"
#include "ShaderState.h"
#include <type_traits>

class RenderTarget;
class InputLayout;
class Buffer;
class PixelBuffer;
class ShaderState;
class Sampler;

enum class RenderAPI
{
	PlatformDefault,
#ifdef BP_PLATFORM_WINDOWS
	DirectX11
#endif
#ifdef BP_PLATFORM_ANDROID
	OPENGL_ES
#endif
};

DEFINE_ENUM_FLAGS(RendererFlags,
	NONE = 0,
	ENABLE_DEBUGGING = 1 << 0
);

class Renderer
{
	friend class Camera;

public:
	static void SetRenderer(Renderer* pRenderer);
	static Renderer* GetRenderer();
	static void Destroy();

	RenderAPI GetCurrentAPI() const { return m_CurrentAPI; }

	virtual ~Renderer() = default;

	virtual void Initialize(RendererFlags flags) = 0;

	virtual void SetViewport(const Viewport& viewport) = 0;
	virtual void SetPrimitiveTopology(PrimitiveTopology topology) = 0;

	virtual void BindRenderTarget(RenderTarget* pRenderTarget) = 0;
	virtual void BindInputLayout(InputLayout* pInputLayout) = 0;
	virtual void BindIndexBuffer(Buffer* pBuffer) = 0;
	virtual void BindVertexBuffer(Buffer* pBuffer) = 0;
	virtual void BindVertexBuffers(const std::vector<Buffer*>& pBuffers) = 0;
	virtual void BindShaderState(ShaderState* pShaderState) = 0;

	virtual void BindConstantBufferVS(Buffer* pBuffer, uint index) = 0;
	virtual void BindConstantBuffersVS(const std::vector<Buffer*>& pBuffers, uint index) = 0;
	virtual void BindConstantBufferPS(Buffer* pBuffer, uint index) = 0;
	virtual void BindConstantBuffersPS(const std::vector<Buffer*>& pBuffers, uint index) = 0;
	virtual void BindConstantBufferCS(Buffer* pBuffer, uint index) = 0;
	virtual void BindConstantBuffersCS(const std::vector<Buffer*>& pBuffers, uint index) = 0;

	virtual void BindSamplerVS(Sampler* pSampler, uint index) = 0;
	virtual void BindSamplersVS(const std::vector<Sampler*>& pSamplers, uint index) = 0;
	virtual void BindSamplerPS(Sampler* pSampler, uint index) = 0;
	virtual void BindSamplersPS(const std::vector<Sampler*>& pSamplers, uint index) = 0;
	virtual void BindSamplerCS(Sampler* pSampler, uint index) = 0;
	virtual void BindSamplersCS(const std::vector<Sampler*>& pSamplers, uint index) = 0;

	virtual void BindReadBufferVS(Buffer* pBuffer, uint index) = 0;
	virtual void BindReadBufferVS(PixelBuffer* pBuffer, uint index) = 0;
	virtual void BindReadBuffersVS(const std::vector<Buffer*>& pBuffers, uint index) = 0;
	virtual void BindReadBuffersVS(const std::vector<PixelBuffer*>& pBuffers, uint index) = 0;
	virtual void BindReadBufferPS(Buffer* pBuffer, uint index) = 0;
	virtual void BindReadBufferPS(PixelBuffer* pBuffer, uint index) = 0;
	virtual void BindReadBuffersPS(const std::vector<Buffer*>& pBuffers, uint index) = 0;
	virtual void BindReadBuffersPS(const std::vector<PixelBuffer*>& pBuffers, uint index) = 0;
	virtual void BindReadBufferCS(Buffer* pBuffer, uint index) = 0;
	virtual void BindReadBufferCS(PixelBuffer* pBuffer, uint index) = 0;
	virtual void BindReadBuffersCS(const std::vector<Buffer*>& pBuffers, uint index) = 0;
	virtual void BindReadBuffersCS(const std::vector<PixelBuffer*>& pBuffers, uint index) = 0;

	virtual void Draw(uint vertexCount) = 0;
	virtual void DrawIndexed(uint indexCount) = 0;

	virtual void ClearRenderTarget(RenderTarget* pRenderTarget, const Color& pClearColor) = 0;

protected:
	Renderer(RenderAPI api);

	virtual void BindConstantBuffer_Internal(ShaderType type, std::vector<Buffer*> pBuffers, uint index) = 0;

protected:
	static Renderer* m_pInstance;
	RenderAPI m_CurrentAPI;
};