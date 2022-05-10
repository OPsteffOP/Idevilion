// Folder: Graphics/Text

#pragma once
#include "Font.h"

class FontCache
{
public:
	static FontCache* GetInstance();
	~FontCache();

	Font* GetFont(const std::string& fontName);

private:
	FontCache() = default;

private:
	std::unordered_map<std::string, Font*> m_pFonts;
};