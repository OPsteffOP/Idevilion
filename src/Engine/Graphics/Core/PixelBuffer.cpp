// Folder: Graphics/Core

#include "EnginePCH.h"
#include "PixelBuffer.h"

#pragma warning(push)
#pragma warning(disable: 4996)
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_ONLY_JPEG
#include "ImageLoading.h"
#include "ImageSaving.h"
#pragma warning(pop)

#include "GraphicsFactory.h"

PixelBuffer::PixelBuffer(uint width, uint height, PixelFormat format, BufferUsage usage, ShaderAccessFlags shaderAccess, CPUAccessFlags cpuAccess, BindFlags bindFlags)
	: m_Width(width)
	, m_Height(height)
	, m_PixelFormat(format)
	, m_BufferUsage(usage)
	, m_ShaderAccess(shaderAccess)
	, m_CPUAccess(cpuAccess)
	, m_BindFlags(bindFlags)
{
	// TODO: VALIDATE ALL PROPERTIES
}

PixelBuffer::PixelBuffer(const std::string& imagePath, BufferUsage usage, ShaderAccessFlags shaderAccess, CPUAccessFlags cpuAccess, BindFlags bindFlags)
	: PixelBuffer(0, 0, PixelFormat(-1), usage, shaderAccess, cpuAccess, bindFlags)
{
	m_ImagePath = imagePath;
}

void PixelBuffer::SaveToTexture(const std::string& path)
{
	bool shouldDeletePixelBuffer = false;
	PixelBuffer* pReadPixelBuffer = this;

	if (!(m_CPUAccess & CPUAccessFlags::READ) || m_BufferUsage != BufferUsage::DYNAMIC)
	{
		pReadPixelBuffer = GraphicsFactory::CreatePixelBuffer(m_Width, m_Height, m_PixelFormat, BufferUsage::STAGING, ShaderAccessFlags::NO_ACCESS, CPUAccessFlags::READ, BindFlags::NONE);
		pReadPixelBuffer->CopyFromPixelBuffer(this);
		shouldDeletePixelBuffer = true;
	}

	// Load buffer from GPU memory to CPU memory
	void* pBuffer = malloc((m_Width * m_Height) * GetBPP(m_PixelFormat));
	pReadPixelBuffer->LoadFromGPU(pBuffer);

	if (shouldDeletePixelBuffer)
		SAFE_DELETE(pReadPixelBuffer);

	// Save CPU memory to texture
	stbi_write_png(path.c_str(), m_Width, m_Height, GetComponents(m_PixelFormat), pBuffer, m_Width * GetBPP(m_PixelFormat));

	free(pBuffer);
}

void PixelBuffer::SaveToBinaryFile(ByteStream& output)
{
	// TODO: code is used for both SaveToTexture functions, reuse instead
	bool shouldDeletePixelBuffer = false;
	PixelBuffer* pReadPixelBuffer = this;

	if (!(m_CPUAccess & CPUAccessFlags::READ) || m_BufferUsage != BufferUsage::DYNAMIC)
	{
		pReadPixelBuffer = GraphicsFactory::CreatePixelBuffer(m_Width, m_Height, m_PixelFormat, BufferUsage::STAGING, ShaderAccessFlags::NO_ACCESS, CPUAccessFlags::READ, BindFlags::NONE);
		pReadPixelBuffer->CopyFromPixelBuffer(this);
		shouldDeletePixelBuffer = true;
	}

	// Load buffer from GPU memory to CPU memory
	void* pBuffer = malloc((m_Width * m_Height) * GetBPP(m_PixelFormat));
	pReadPixelBuffer->LoadFromGPU(pBuffer);

	if (shouldDeletePixelBuffer)
		SAFE_DELETE(pReadPixelBuffer);

	// Generating PNG data and saving to file
	int pngDataLength;
	unsigned char* pPngData = stbi_write_png_to_mem(static_cast<unsigned char*>(pBuffer), m_Width * GetBPP(m_PixelFormat), m_Width, m_Height, GetComponents(m_PixelFormat), &pngDataLength);
	output.WriteInt32(pngDataLength);
	output.WriteData(pPngData, (uint)pngDataLength, Endian::BIG_ENDIAN);
	STBIW_FREE(pPngData);

	free(pBuffer);
}

void* PixelBuffer::LoadImage(const std::string& imagePath, uint& width, uint& height, PixelFormat& format) const
{
	if (!std::filesystem::exists(imagePath))
	{
		LOG_ERROR("Image file path does not exists, path=%s", imagePath.c_str());
		return nullptr;
	}

	int widthInt;
	int heightInt;
	int channelCount;
	float* pData = stbi_loadf(imagePath.c_str(), &widthInt, &heightInt, &channelCount, 0);

	if (channelCount != 3 && channelCount != 4)
	{
		LOG_ERROR("Unknown image format, channelCount=%d, required=3/4", channelCount);
		return nullptr;
	}

	width = (uint)widthInt;
	height = (uint)heightInt;
	format = (channelCount == 3) ? PixelFormat::R32G32B32_FLOAT : PixelFormat::R32G32B32A32_FLOAT;

	return (void*)pData;
}

void PixelBuffer::FreeImage(void* pImagePtr)
{
	stbi_image_free(pImagePtr);
}