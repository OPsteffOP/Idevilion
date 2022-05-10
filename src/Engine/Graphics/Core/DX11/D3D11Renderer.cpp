// Folder: Graphics/Core/DX11

#include "EnginePCH.h"
#include "D3D11Renderer.h"

#ifdef BP_PLATFORM_WINDOWS

#include "D3D11RenderTarget.h"
#include "D3D11InputLayout.h"
#include "D3D11Buffer.h"
#include "D3D11PixelBuffer.h"
#include "D3D11ShaderState.h"
#include "D3D11Sampler.h"

D3D11Renderer::D3D11Renderer()
	: Renderer(RenderAPI::DirectX11)
	, m_pDevice(nullptr)
	, m_pContext(nullptr)
{}

D3D11Renderer::~D3D11Renderer()
{
	// TODO: remove - this is just for temporary testing, this needs to be abstracted
	SAFE_RELEASE(m_pDepthStencilState);
	SAFE_RELEASE(m_pBlendState);
	// TODO: remove - this is just for temporary testing, this needs to be abstracted

	SAFE_RELEASE(m_pContext);
	SAFE_RELEASE(m_pDevice);
}

void D3D11Renderer::Initialize(RendererFlags flags)
{
	// TODO: find the dedicated graphics card for the adapter parameter
	HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, GetDeviceFlags(flags), nullptr,
		0, D3D11_SDK_VERSION, &m_pDevice, nullptr, &m_pContext);
	if (FAILED(hr))
	{
		LOG_ERROR("Failed to initialize D3D11 device, error=%d", (int)hr);
		return;
	}

	// TODO: remove - this is just for temporary testing, this needs to be abstracted
	//D3D11_DEPTH_STENCIL_DESC dsDesc;
	//dsDesc.DepthEnable = false;
	//dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	//dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	//dsDesc.StencilEnable = false;
	//dsDesc.StencilReadMask = 0xFF;
	//dsDesc.StencilWriteMask = 0xFF;
	//dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	//dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	//dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	//dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	//dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	//dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	//dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	//dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	//m_pDevice->CreateDepthStencilState(&dsDesc, &m_pDepthStencilState);
	//m_pContext->OMSetDepthStencilState(m_pDepthStencilState, 0);
	// TODO: remove - this is just for temporary testing, this needs to be abstracted

	// TODO: remove - this is just for temporary testing, this needs to be abstracted
	//D3D11_BLEND_DESC omDesc;
	//ZeroMemory(&omDesc, sizeof(D3D11_BLEND_DESC));
	//omDesc.RenderTarget[0].BlendEnable = true;
	//omDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	//omDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	//omDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	//omDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	//omDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	//omDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	//omDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	//m_pDevice->CreateBlendState(&omDesc, &m_pBlendState);
	//m_pContext->OMSetBlendState(m_pBlendState, 0, 0xffffffff);
	// TODO: remove - this is just for temporary testing, this needs to be abstracted
}

void D3D11Renderer::SetViewport(const Viewport& viewport)
{
	m_pContext->RSSetViewports(1, reinterpret_cast<const D3D11_VIEWPORT*>(&viewport));
}

void D3D11Renderer::SetPrimitiveTopology(PrimitiveTopology topology)
{
	m_pContext->IASetPrimitiveTopology(ConvertPrimitiveTopology(topology));
}

void D3D11Renderer::BindRenderTarget(RenderTarget* pRenderTarget)
{
	if (pRenderTarget == nullptr)
	{
		m_pContext->OMSetRenderTargets(0, NULL, NULL);
		return;
	}

	D3D11RenderTarget* pD3D11RenderTarget = static_cast<D3D11RenderTarget*>(pRenderTarget);
	ID3D11RenderTargetView* pRTV = pD3D11RenderTarget->m_pRTV;
	ID3D11DepthStencilView* pDSV = pD3D11RenderTarget->m_pDSV;

	m_pContext->OMSetRenderTargets(1, &pRTV, pDSV);
}

void D3D11Renderer::BindInputLayout(InputLayout* pInputLayout)
{
	D3D11InputLayout* pD3D11InputLayout = static_cast<D3D11InputLayout*>(pInputLayout);
	m_pContext->IASetInputLayout(pD3D11InputLayout->m_pInputLayout);
}

void D3D11Renderer::BindIndexBuffer(Buffer* pBuffer)
{
	D3D11Buffer* pD3D11Buffer = static_cast<D3D11Buffer*>(pBuffer);

	if (!(pD3D11Buffer->m_BindFlags & BindFlags::INDEX_BUFFER))
	{
		LOG_ERROR("Trying to bind an index buffer that doesn't have the index buffer bind flag");
		return;
	}

	DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
	if (pD3D11Buffer->m_ElementSize == sizeof(int16_t))
		format = DXGI_FORMAT_R16_UINT;
	else if (pD3D11Buffer->m_ElementSize == sizeof(int16_t))
		format = DXGI_FORMAT_R32_UINT;

	m_pContext->IASetIndexBuffer(pD3D11Buffer->m_pBuffer, format, 0);
}

void D3D11Renderer::BindVertexBuffer(Buffer* pBuffer)
{
	BindVertexBuffers({ pBuffer });
}

void D3D11Renderer::BindVertexBuffers(const std::vector<Buffer*>& pBuffers)
{
	std::vector<ID3D11Buffer*> pD3D11Buffers;
	std::vector<UINT> strides;
	std::vector<UINT> offsets;

	pD3D11Buffers.reserve(pBuffers.size());
	strides.reserve(pBuffers.size());
	offsets.reserve(pBuffers.size());

	for (Buffer* pBuffer : pBuffers)
	{
		D3D11Buffer* pD3D11Buffer = static_cast<D3D11Buffer*>(pBuffer);

		if (!(pD3D11Buffer->m_BindFlags & BindFlags::VERTEX_BUFFER))
		{
			LOG_ERROR("Trying to bind a vertex buffer that doesn't have the vertex buffer bind flag");
			return;
		}

		pD3D11Buffers.push_back(pD3D11Buffer->m_pBuffer);
		strides.push_back(pD3D11Buffer->m_ElementSize);
		offsets.push_back(0);
	}

	m_pContext->IASetVertexBuffers(0, (UINT)pBuffers.size(), pD3D11Buffers.data(), strides.data(), offsets.data());
}

void D3D11Renderer::BindShaderState(ShaderState* pShaderState)
{
	D3D11ShaderState* pD3D11ShaderState = static_cast<D3D11ShaderState*>(pShaderState);

	switch (pD3D11ShaderState->m_Type)
	{
	case ShaderType::VERTEX_SHADER:
		m_pContext->VSSetShader(pD3D11ShaderState->m_pVertexShader, NULL, 0);
		break;
	case ShaderType::PIXEL_SHADER:
		m_pContext->PSSetShader(pD3D11ShaderState->m_pPixelShader, NULL, 0);
		break;
	case ShaderType::COMPUTE_SHADER:
		m_pContext->CSSetShader(pD3D11ShaderState->m_pComputeShader, NULL, 0);
		break;
	}
}

void D3D11Renderer::BindConstantBufferVS(Buffer* pBuffer, uint index)
{
	BindConstantBuffer_Internal(ShaderType::VERTEX_SHADER, { pBuffer }, (uint)ConstantBufferIndex::USER_INDEX + index);
}

void D3D11Renderer::BindConstantBuffersVS(const std::vector<Buffer*>& pBuffers, uint index)
{
	BindConstantBuffer_Internal(ShaderType::VERTEX_SHADER, pBuffers, (uint)ConstantBufferIndex::USER_INDEX + index);
}

void D3D11Renderer::BindConstantBufferPS(Buffer* pBuffer, uint index)
{
	BindConstantBuffer_Internal(ShaderType::PIXEL_SHADER, { pBuffer }, (uint)ConstantBufferIndex::USER_INDEX + index);
}

void D3D11Renderer::BindConstantBuffersPS(const std::vector<Buffer*>& pBuffers, uint index)
{
	BindConstantBuffer_Internal(ShaderType::PIXEL_SHADER, pBuffers, (uint)ConstantBufferIndex::USER_INDEX + index);
}

void D3D11Renderer::BindConstantBufferCS(Buffer* pBuffer, uint index)
{
	BindConstantBuffer_Internal(ShaderType::COMPUTE_SHADER, { pBuffer }, (uint)ConstantBufferIndex::USER_INDEX + index);
}

void D3D11Renderer::BindConstantBuffersCS(const std::vector<Buffer*>& pBuffers, uint index)
{
	BindConstantBuffer_Internal(ShaderType::COMPUTE_SHADER, pBuffers, (uint)ConstantBufferIndex::USER_INDEX + index);
}

void D3D11Renderer::BindConstantBuffer_Internal(ShaderType type, std::vector<Buffer*> pBuffers, uint index)
{
	std::vector<ID3D11Buffer*> pD3D11Buffers;
	for (Buffer* pBuffer : pBuffers)
	{
		pD3D11Buffers.push_back(static_cast<D3D11Buffer*>(pBuffer)->m_pBuffer);
	}

	switch (type)
	{
	case ShaderType::VERTEX_SHADER:
		m_pContext->VSSetConstantBuffers(index, (UINT)pD3D11Buffers.size(), pD3D11Buffers.data());
		break;
	case ShaderType::PIXEL_SHADER:
		m_pContext->PSSetConstantBuffers(index, (UINT)pD3D11Buffers.size(), pD3D11Buffers.data());
		break;
	case ShaderType::COMPUTE_SHADER:
		m_pContext->CSSetConstantBuffers(index, (UINT)pD3D11Buffers.size(), pD3D11Buffers.data());
		break;
	}
}

void D3D11Renderer::BindSamplerVS(Sampler* pSampler, uint index)
{
	BindSamplersVS({ pSampler }, index);
}

void D3D11Renderer::BindSamplersVS(const std::vector<Sampler*>& pSamplers, uint index)
{
	std::vector<ID3D11SamplerState*> pD3D11Samplers;
	for (Sampler* pSampler : pSamplers)
	{
		pD3D11Samplers.push_back(static_cast<D3D11Sampler*>(pSampler)->m_pSamplerState);
	}

	m_pContext->VSSetSamplers(index, (UINT)pD3D11Samplers.size(), pD3D11Samplers.data());
}

void D3D11Renderer::BindSamplerPS(Sampler* pSampler, uint index)
{
	BindSamplersPS({ pSampler }, index);
}

void D3D11Renderer::BindSamplersPS(const std::vector<Sampler*>& pSamplers, uint index)
{
	std::vector<ID3D11SamplerState*> pD3D11Samplers;
	for (Sampler* pSampler : pSamplers)
	{
		pD3D11Samplers.push_back(static_cast<D3D11Sampler*>(pSampler)->m_pSamplerState);
	}

	m_pContext->PSSetSamplers(index, (UINT)pD3D11Samplers.size(), pD3D11Samplers.data());
}

void D3D11Renderer::BindSamplerCS(Sampler* pSampler, uint index)
{
	BindSamplersCS({ pSampler }, index);
}

void D3D11Renderer::BindSamplersCS(const std::vector<Sampler*>& pSamplers, uint index)
{
	std::vector<ID3D11SamplerState*> pD3D11Samplers;
	for (Sampler* pSampler : pSamplers)
	{
		pD3D11Samplers.push_back(static_cast<D3D11Sampler*>(pSampler)->m_pSamplerState);
	}

	m_pContext->CSSetSamplers(index, (UINT)pD3D11Samplers.size(), pD3D11Samplers.data());
}

void D3D11Renderer::BindReadBufferVS(Buffer* pBuffer, uint index)
{
	BindReadBuffersVS({ pBuffer }, index);
}

void D3D11Renderer::BindReadBufferVS(PixelBuffer* pBuffer, uint index)
{
	BindReadBuffersVS({ pBuffer }, index);
}

void D3D11Renderer::BindReadBuffersVS(const std::vector<Buffer*>& pBuffers, uint index)
{
	std::vector<ID3D11ShaderResourceView*> pD3D11SRVs;
	for (Buffer* pBuffer : pBuffers)
	{
		D3D11Buffer* pD3D11Buffer = static_cast<D3D11Buffer*>(pBuffer);

		if (!(pD3D11Buffer->m_ShaderAccess & ShaderAccessFlags::READ))
		{
			LOG_ERROR("Trying to bind buffer as SRV while the buffer does not have the ShaderAccessFlags::READ flag");
			continue;
		}

		pD3D11SRVs.push_back(pD3D11Buffer->m_pSRV);
	}

	m_pContext->VSSetShaderResources(index, (UINT)pD3D11SRVs.size(), pD3D11SRVs.data());
}

void D3D11Renderer::BindReadBuffersVS(const std::vector<PixelBuffer*>& pBuffers, uint index)
{
	std::vector<ID3D11ShaderResourceView*> pD3D11SRVs;
	for (PixelBuffer* pBuffer : pBuffers)
	{
		D3D11PixelBuffer* pD3D11Buffer = static_cast<D3D11PixelBuffer*>(pBuffer);

		if (!(pD3D11Buffer->m_ShaderAccess & ShaderAccessFlags::READ))
		{
			LOG_ERROR("Trying to bind buffer as SRV while the buffer does not have the ShaderAccessFlags::READ flag");
			continue;
		}

		pD3D11SRVs.push_back(pD3D11Buffer->m_pSRV);
	}

	m_pContext->VSSetShaderResources(index, (UINT)pD3D11SRVs.size(), pD3D11SRVs.data());
}

void D3D11Renderer::BindReadBufferPS(Buffer* pBuffer, uint index)
{
	BindReadBuffersPS({ pBuffer }, index);
}

void D3D11Renderer::BindReadBufferPS(PixelBuffer* pBuffer, uint index)
{
	BindReadBuffersPS({ pBuffer }, index);
}

void D3D11Renderer::BindReadBuffersPS(const std::vector<Buffer*>& pBuffers, uint index)
{
	std::vector<ID3D11ShaderResourceView*> pD3D11SRVs;
	for (Buffer* pBuffer : pBuffers)
	{
		D3D11Buffer* pD3D11Buffer = static_cast<D3D11Buffer*>(pBuffer);

		if (!(pD3D11Buffer->m_ShaderAccess & ShaderAccessFlags::READ))
		{
			LOG_ERROR("Trying to bind buffer as SRV while the buffer does not have the ShaderAccessFlags::READ flag");
			continue;
		}

		pD3D11SRVs.push_back(pD3D11Buffer->m_pSRV);
	}

	m_pContext->PSSetShaderResources(index, (UINT)pD3D11SRVs.size(), pD3D11SRVs.data());
}

void D3D11Renderer::BindReadBuffersPS(const std::vector<PixelBuffer*>& pBuffers, uint index)
{
	std::vector<ID3D11ShaderResourceView*> pD3D11SRVs;
	for (PixelBuffer* pBuffer : pBuffers)
	{
		D3D11PixelBuffer* pD3D11Buffer = static_cast<D3D11PixelBuffer*>(pBuffer);

		if (!(pD3D11Buffer->m_ShaderAccess & ShaderAccessFlags::READ))
		{
			LOG_ERROR("Trying to bind buffer as SRV while the buffer does not have the ShaderAccessFlags::READ flag");
			continue;
		}

		pD3D11SRVs.push_back(pD3D11Buffer->m_pSRV);
	}

	m_pContext->PSSetShaderResources(index, (UINT)pD3D11SRVs.size(), pD3D11SRVs.data());
}

void D3D11Renderer::BindReadBufferCS(Buffer* pBuffer, uint index)
{
	BindReadBuffersCS({ pBuffer }, index);
}

void D3D11Renderer::BindReadBufferCS(PixelBuffer* pBuffer, uint index)
{
	BindReadBuffersCS({ pBuffer }, index);
}

void D3D11Renderer::BindReadBuffersCS(const std::vector<Buffer*>& pBuffers, uint index)
{
	std::vector<ID3D11ShaderResourceView*> pD3D11SRVs;
	for (Buffer* pBuffer : pBuffers)
	{
		D3D11Buffer* pD3D11Buffer = static_cast<D3D11Buffer*>(pBuffer);

		if (!(pD3D11Buffer->m_ShaderAccess & ShaderAccessFlags::READ))
		{
			LOG_ERROR("Trying to bind buffer as SRV while the buffer does not have the ShaderAccessFlags::READ flag");
			continue;
		}

		pD3D11SRVs.push_back(pD3D11Buffer->m_pSRV);
	}

	m_pContext->CSSetShaderResources(index, (UINT)pD3D11SRVs.size(), pD3D11SRVs.data());
}

void D3D11Renderer::BindReadBuffersCS(const std::vector<PixelBuffer*>& pBuffers, uint index)
{
	std::vector<ID3D11ShaderResourceView*> pD3D11SRVs;
	for (PixelBuffer* pBuffer : pBuffers)
	{
		D3D11PixelBuffer* pD3D11Buffer = static_cast<D3D11PixelBuffer*>(pBuffer);

		if (!(pD3D11Buffer->m_ShaderAccess & ShaderAccessFlags::READ))
		{
			LOG_ERROR("Trying to bind buffer as SRV while the buffer does not have the ShaderAccessFlags::READ flag");
			continue;
		}

		pD3D11SRVs.push_back(pD3D11Buffer->m_pSRV);
	}

	m_pContext->CSSetShaderResources(index, (UINT)pD3D11SRVs.size(), pD3D11SRVs.data());
}

void D3D11Renderer::Draw(uint vertexCount)
{
	m_pContext->Draw(vertexCount, 0);
}

void D3D11Renderer::DrawIndexed(uint indexCount)
{
	m_pContext->DrawIndexed(indexCount, 0, 0);
}

void D3D11Renderer::ClearRenderTarget(RenderTarget* pRenderTarget, const Color& pClearColor)
{
	D3D11RenderTarget* pD3D11RenderTarget = static_cast<D3D11RenderTarget*>(pRenderTarget);

	if (pD3D11RenderTarget->m_pRTV != nullptr)
		m_pContext->ClearRenderTargetView(pD3D11RenderTarget->m_pRTV, &pClearColor.r);

	if (pD3D11RenderTarget->m_pDSV != nullptr)
		m_pContext->ClearDepthStencilView(pD3D11RenderTarget->m_pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}

unsigned int D3D11Renderer::GetDeviceFlags(RendererFlags flags) const
{
	uint deviceFlags = 0;

	if (flags & RendererFlags::ENABLE_DEBUGGING)
		deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;

	return deviceFlags;
}

#endif