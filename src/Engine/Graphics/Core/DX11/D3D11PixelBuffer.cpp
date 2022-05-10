// Folder: Graphics/Core/DX11

#include "EnginePCH.h"
#include "D3D11PixelBuffer.h"

#ifdef BP_PLATFORM_WINDOWS

#include "D3D11Renderer.h"

D3D11PixelBuffer::D3D11PixelBuffer(uint width, uint height, PixelFormat format, BufferUsage usage, ShaderAccessFlags shaderAccess, CPUAccessFlags cpuAccess, BindFlags bindFlags)
	: PixelBuffer(width, height, format, usage, shaderAccess, cpuAccess, bindFlags)
	, m_pTexture(nullptr)
	, m_pSRV(nullptr)
	, m_pUAV(nullptr)
{
	Initialize(nullptr, width, height, format, usage, shaderAccess, cpuAccess, bindFlags);
}

D3D11PixelBuffer::D3D11PixelBuffer(void* pData, uint width, uint height, PixelFormat format, BufferUsage usage, ShaderAccessFlags shaderAccess, CPUAccessFlags cpuAccess, BindFlags bindFlags)
	: PixelBuffer(width, height, format, usage, shaderAccess, cpuAccess, bindFlags)
	, m_pTexture(nullptr)
	, m_pSRV(nullptr)
	, m_pUAV(nullptr)
{
	Initialize(pData, width, height, format, usage, shaderAccess, cpuAccess, bindFlags);
}

D3D11PixelBuffer::D3D11PixelBuffer(const std::string& imagePath, BufferUsage usage, ShaderAccessFlags shaderAccess, CPUAccessFlags cpuAccess, BindFlags bindFlags)
	: PixelBuffer(imagePath, usage, shaderAccess, cpuAccess, bindFlags)
	, m_pTexture(nullptr)
	, m_pSRV(nullptr)
	, m_pUAV(nullptr)
{
	void* pData = LoadImage(imagePath, m_Width, m_Height, m_PixelFormat);
	Initialize(pData, m_Width, m_Height, m_PixelFormat, usage, shaderAccess, cpuAccess, bindFlags);
	FreeImage(pData);
}

D3D11PixelBuffer::D3D11PixelBuffer(ID3D11Texture2D* pTexture, uint width, uint height, PixelFormat format, BufferUsage usage, ShaderAccessFlags shaderAccess, CPUAccessFlags cpuAccess, BindFlags bindFlags)
	: PixelBuffer(width, height, format, usage, shaderAccess, cpuAccess, bindFlags)
	, m_pTexture(pTexture)
	, m_pSRV(nullptr)
	, m_pUAV(nullptr)
{
	Initialize(nullptr, width, height, format, usage, shaderAccess, cpuAccess, bindFlags);
}

void D3D11PixelBuffer::Initialize(void* pData, uint width, uint height, PixelFormat format, BufferUsage usage, ShaderAccessFlags shaderAccess, CPUAccessFlags cpuAccess, BindFlags bindFlags)
{
	HRESULT hr;

	D3D11Renderer* pRenderer = static_cast<D3D11Renderer*>(Renderer::GetRenderer());

	if (m_pTexture == nullptr)
	{
		DXGI_FORMAT texturePixelFormat = GetDepthTextureFormat(format);
		if (texturePixelFormat == DXGI_FORMAT_UNKNOWN)
			texturePixelFormat = ConvertPixelFormat(format);

		D3D11_TEXTURE2D_DESC textureDescriptor{};
		textureDescriptor.Width = width;
		textureDescriptor.Height = height;
		textureDescriptor.MipLevels = 1; // no mipmapping
		textureDescriptor.ArraySize = 1;
		textureDescriptor.Format = texturePixelFormat;
		textureDescriptor.SampleDesc.Count = 1; // no anti-aliasing
		textureDescriptor.SampleDesc.Quality = 0;
		textureDescriptor.Usage = ConvertBufferUsage(usage);
		textureDescriptor.BindFlags = ConvertBindFlags(bindFlags, shaderAccess);
		textureDescriptor.CPUAccessFlags = ConvertCPUAccess(cpuAccess);
		textureDescriptor.MiscFlags = ConvertMiscFlags(bindFlags);

		// Create the init data for the texture
		D3D11_SUBRESOURCE_DATA initData{};
		initData.pSysMem = pData;
		initData.SysMemPitch = width * GetBPP(format);

		// Create the texture for directx usage
		hr = pRenderer->m_pDevice->CreateTexture2D(&textureDescriptor, (pData != nullptr) ? &initData : NULL, &m_pTexture);
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create D3D11 Texture2D");
			return;
		}
	}

	if (shaderAccess & ShaderAccessFlags::READ)
	{
		DXGI_FORMAT srvPixelFormat = GetDepthShaderViewFormat(format);
		if (srvPixelFormat == DXGI_FORMAT_UNKNOWN)
			srvPixelFormat = ConvertPixelFormat(format);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDescriptor{};
		srvDescriptor.Format = srvPixelFormat;
		srvDescriptor.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDescriptor.Texture2D.MostDetailedMip = 0;
		srvDescriptor.Texture2D.MipLevels = 1;

		hr = pRenderer->m_pDevice->CreateShaderResourceView(m_pTexture, &srvDescriptor, &m_pSRV);
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create SRV for D3D11 Texture2D");
			return;
		}
	}

	if (shaderAccess & ShaderAccessFlags::WRITE)
	{
		DXGI_FORMAT uavPixelFormat = GetDepthShaderViewFormat(format);
		if (uavPixelFormat == DXGI_FORMAT_UNKNOWN)
			uavPixelFormat = ConvertPixelFormat(format);

		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDescriptor{};
		uavDescriptor.Format = uavPixelFormat;
		uavDescriptor.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDescriptor.Texture2D.MipSlice = 0;

		hr = pRenderer->m_pDevice->CreateUnorderedAccessView(m_pTexture, &uavDescriptor, &m_pUAV);
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create UAV for D3D11 Texture2D");
			return;
		}
	}
}

D3D11PixelBuffer::~D3D11PixelBuffer()
{
	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_pSRV);
	SAFE_RELEASE(m_pUAV);
}

void D3D11PixelBuffer::CopyFromPixelBuffer(PixelBuffer* pSrcBuffer)
{
	D3D11Renderer* pRenderer = static_cast<D3D11Renderer*>(Renderer::GetRenderer());
	D3D11PixelBuffer* pD3D11SrcBuffer = static_cast<D3D11PixelBuffer*>(pSrcBuffer);
	pRenderer->m_pContext->CopyResource(m_pTexture, pD3D11SrcBuffer->m_pTexture);
}

void D3D11PixelBuffer::LoadFromGPU(void* pData)
{
	if (!(m_CPUAccess & CPUAccessFlags::READ))
	{
		LOG_ERROR("LoadFromGPU isn't available for a buffer that isn't created with CPUAccessFlags::READ");
		return;
	}

	D3D11Renderer* pRenderer = static_cast<D3D11Renderer*>(Renderer::GetRenderer());

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	pRenderer->m_pContext->Map(m_pTexture, 0, D3D11_MAP_READ, 0, &mappedResource);
	memcpy(pData, mappedResource.pData, (m_Width * m_Height) * GetBPP(m_PixelFormat));
	pRenderer->m_pContext->Unmap(m_pTexture, 0);
}

#endif