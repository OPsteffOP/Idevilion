// Folder: Graphics/Core/DX11

#include "EnginePCH.h"
#include "D3D11Buffer.h"

#ifdef BP_PLATFORM_WINDOWS

#include "D3D11Renderer.h"

D3D11Buffer::D3D11Buffer(uint elementSize, uint elementCount, BufferUsage usage, ShaderAccessFlags shaderAccess, CPUAccessFlags cpuAccess, BindFlags bindFlags)
	: D3D11Buffer(nullptr, elementSize, elementCount, usage, shaderAccess, cpuAccess, bindFlags)
{}

D3D11Buffer::D3D11Buffer(const void* pData, uint elementSize, uint elementCount, BufferUsage usage, ShaderAccessFlags shaderAccess, CPUAccessFlags cpuAccess, BindFlags bindFlags)
	: Buffer(elementSize, elementCount, usage, shaderAccess, cpuAccess, bindFlags)
	, m_pBuffer(nullptr)
	, m_pSRV(nullptr)
	, m_pUAV(nullptr)
{
	if (!Validate())
		return;

	if (pData == nullptr && usage == BufferUsage::IMMUTABLE)
	{
		LOG_ERROR("Trying to create an immutable buffer without specifying initial data");
		return;
	}

	HRESULT hr;

	D3D11Renderer* pRenderer = static_cast<D3D11Renderer*>(Renderer::GetRenderer());

	D3D11_BUFFER_DESC bufferDescriptor{};
	bufferDescriptor.ByteWidth = elementSize * elementCount;
	bufferDescriptor.Usage = ConvertBufferUsage(usage);
	bufferDescriptor.BindFlags = ConvertBindFlags(bindFlags, shaderAccess);
	bufferDescriptor.CPUAccessFlags = ConvertCPUAccess(cpuAccess);
	bufferDescriptor.MiscFlags = 0;
	bufferDescriptor.StructureByteStride = elementSize;

	D3D11_SUBRESOURCE_DATA initData{};
	initData.pSysMem = pData;

	hr = pRenderer->m_pDevice->CreateBuffer(&bufferDescriptor, (pData != nullptr) ? &initData : NULL, &m_pBuffer);
	if (FAILED(hr))
	{
		LOG_ERROR("Failed to create D3D11 Buffer");
		return;
	}

	if (shaderAccess & ShaderAccessFlags::READ)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDescriptor{};
		srvDescriptor.Format = DXGI_FORMAT_UNKNOWN;
		srvDescriptor.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
		srvDescriptor.BufferEx.FirstElement = 0;
		srvDescriptor.BufferEx.NumElements = elementCount;
		srvDescriptor.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;

		hr = pRenderer->m_pDevice->CreateShaderResourceView(m_pBuffer, &srvDescriptor, &m_pSRV);
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create SRV for D3D11 Buffer");
			return;
		}
	}

	if (shaderAccess & ShaderAccessFlags::WRITE)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDescriptor{};
		uavDescriptor.Format = DXGI_FORMAT_UNKNOWN;
		uavDescriptor.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDescriptor.Buffer.FirstElement = 0;
		uavDescriptor.Buffer.NumElements = elementCount;
		uavDescriptor.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;

		hr = pRenderer->m_pDevice->CreateUnorderedAccessView(m_pBuffer, &uavDescriptor, &m_pUAV);
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create UAV for D3D11 Buffer");
			return;
		}
	}
}

D3D11Buffer::~D3D11Buffer()
{
	SAFE_RELEASE(m_pBuffer);
	SAFE_RELEASE(m_pSRV);
	SAFE_RELEASE(m_pUAV);
}

void D3D11Buffer::UpdateData(const void* pData)
{
	UpdateData(pData, m_ElementCount * m_ElementSize);
}

void D3D11Buffer::UpdateData(const void* pData, uint size)
{
	if (size > m_ElementCount * m_ElementSize)
	{
		LOG_ERROR("Trying to update a buffer's data with size greater than the allocated buffer size");
		return;
	}

	D3D11Renderer* pRenderer = static_cast<D3D11Renderer*>(Renderer::GetRenderer());

	switch (m_BufferUsage)
	{
	case BufferUsage::DEFAULT:
		// TODO: UpdateSubResource
		// https://docs.microsoft.com/en-us/windows/win32/api/d3d11_1/nf-d3d11_1-id3d11devicecontext1-updatesubresource1
		break;
	case BufferUsage::DYNAMIC:
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

		pRenderer->m_pContext->Map(m_pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy(mappedResource.pData, pData, size);
		pRenderer->m_pContext->Unmap(m_pBuffer, 0);

		break;
	}
	default:
		LOG_ERROR("Trying to update a buffer that doesn't have BufferUsage::DEFAULT or BufferUsage::DYNAMIC set");
		break;
	}
}

bool D3D11Buffer::Validate() const
{
	if (m_BindFlags & BindFlags::CONSTANT_BUFFER && m_BufferUsage == BufferUsage::DEFAULT)
	{
		LOG_ERROR("Trying to bind a D3D11 constant buffer with BufferUsage::DEFAULT, use BufferUsage::DYNAMIC for a constant buffer instead");
		return false;
	}

	return true;
}

#endif