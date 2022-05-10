// Folder: IO/Image

#include "EnginePCH.h"
#include "ImageCache.h"

#include "GraphicsFactory.h"

ImageCache* ImageCache::GetInstance()
{
	static ImageCache* pInstance = nullptr;
	if (pInstance == nullptr)
	{
		ALLOW_NEXT_ALLOCATION_LEAK();
		pInstance = new ImageCache();
	}

	return pInstance;
}

ImageCache::~ImageCache()
{
	for (std::pair<const std::string, PixelBuffer*>& pair : m_pPixelBuffers)
	{
		SAFE_DELETE(pair.second);
	}
}

PixelBuffer* ImageCache::GetImage(const std::string& path)
{
	if (m_pPixelBuffers.find(path) == m_pPixelBuffers.end())
	{
		if (!std::filesystem::exists(path))
		{
			LOG_ERROR("Trying to access image that doesn't exists on disk, path=%s", path.c_str());
			return nullptr;
		}

		m_pPixelBuffers[path] = GraphicsFactory::CreatePixelBuffer(path, BufferUsage::IMMUTABLE, ShaderAccessFlags::READ, CPUAccessFlags::NO_ACCESS, BindFlags::NONE);
	}

	return m_pPixelBuffers[path];
}