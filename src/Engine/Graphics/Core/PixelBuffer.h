// Folder: Graphics/Core

#pragma once
#include "BufferEnums.h"
#include "ByteStream.h"

class PixelBuffer
{
public:
	PixelBuffer(uint width, uint height, PixelFormat format, BufferUsage usage, ShaderAccessFlags shaderAccess, CPUAccessFlags cpuAccess, BindFlags bindFlags);
	PixelBuffer(const std::string& imagePath, BufferUsage usage, ShaderAccessFlags shaderAccess, CPUAccessFlags cpuAccess, BindFlags bindFlags);

	virtual ~PixelBuffer() = default;

	virtual void CopyFromPixelBuffer(PixelBuffer* pSrcBuffer) = 0;

	void SaveToTexture(const std::string& path);
	void SaveToBinaryFile(ByteStream& output);

	uint GetWidth() const { return m_Width; }
	uint GetHeight() const { return m_Height; }

	const std::string& GetImagePath() const { return m_ImagePath; }

protected:
	virtual void LoadFromGPU(void* pData) = 0;
	void* LoadImage(const std::string& imagePath, uint& width, uint& height, PixelFormat& format) const;
	void FreeImage(void* pImagePtr);

protected:
	uint m_Width;
	uint m_Height;
	PixelFormat m_PixelFormat;
	const BufferUsage m_BufferUsage;
	const ShaderAccessFlags m_ShaderAccess;
	const CPUAccessFlags m_CPUAccess;
	const BindFlags m_BindFlags;

	std::string m_ImagePath;
};