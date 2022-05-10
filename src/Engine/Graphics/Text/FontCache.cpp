// Folder: Graphics/Text

#include "EnginePCH.h"
#include "FontCache.h"

#ifdef DEV_BUILD
#include "FontBaking.h"
#endif

FontCache* FontCache::GetInstance()
{
	static FontCache* pInstance = nullptr;
	if (pInstance == nullptr)
	{
		ALLOW_NEXT_ALLOCATION_LEAK();
		pInstance = new FontCache();
	}

	return pInstance;
}

FontCache::~FontCache()
{
	for (std::pair<const std::string, Font*>& pair : m_pFonts)
	{
		SAFE_DELETE(pair.second);
	}
}

Font* FontCache::GetFont(const std::string& fontName)
{
	if (m_pFonts.find(fontName) == m_pFonts.end())
	{
		const std::string bpfPath = Paths::Data::DATA_DIR + Paths::Data::FONTS_DIR + fontName + Paths::Data::Extensions::BAKED_SDF_FONT_FILE;
		if (std::filesystem::exists(bpfPath))
		{
			m_pFonts[fontName] = new Font(bpfPath);
		}
#ifdef DEV_BUILD
		else if (std::filesystem::exists(Paths::Data::DATA_DEV_DIR + Paths::Data::FONTS_DIR + fontName + ".ttf"))
		{
			LOG_WARNING("Trying to access font that isn't baked yet, baking it right now because it's a dev build (font found in data-dev folder), font_name=%s", fontName.c_str());
			Baking::BakeFontSDFBitmap(fontName);
			m_pFonts[fontName] = new Font(bpfPath);
		}
		else if (std::filesystem::exists(Paths::OS::GetFontsFolder() + fontName + ".ttf"))
		{
			LOG_WARNING("Trying to access font that isn't baked yet, baking it right now because it's a dev build (font found in OS folder), font_name=%s", fontName.c_str());
			Baking::BakeFontSDFBitmap(fontName);
			m_pFonts[fontName] = new Font(bpfPath);
		}
#endif
		else
		{
			LOG_ERROR("Trying to access a font that doesn't exists, font_name=%s", fontName.c_str());
			return nullptr;
		}
	}

	return m_pFonts[fontName];
}