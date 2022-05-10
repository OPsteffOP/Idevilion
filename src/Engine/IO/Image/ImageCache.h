// Folder: IO/Image

#pragma once

class PixelBuffer;

class ImageCache
{
public:
	static ImageCache* GetInstance();
	~ImageCache();

	PixelBuffer* GetImage(const std::string& path);

private:
	ImageCache() = default;

private:
	std::unordered_map<std::string, PixelBuffer*> m_pPixelBuffers;
};