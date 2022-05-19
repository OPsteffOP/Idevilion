// Folder: IO/Paths

#pragma once

namespace Paths
{
	inline std::string COMPANY_NAME = "Steff";
	inline std::string GAME_NAME = "Placeholder";

	namespace OS
	{
		std::string GetLocalAppFolder();
		std::string GetRoamingAppFolder();

		std::string GetProgramFilesFolder();

		std::string GetLocalDesktopFolder();
		std::string GetFontsFolder();
	}

	namespace Data
	{
		inline std::string CORE_DIR = "";

		inline std::string SHADER_CACHE_DIR = "";
		inline std::string DATA_DIR = "";

#ifdef DEV_BUILD
		inline std::string DATA_DEV_DIR = "C:\\Users\\Administrator\\Perforce\\STEFF-DESKTOP\\P3-Placeholder\\Dev\\data-dev\\";
#endif

		inline constexpr static const char* TEXTURES_DIR = "Textures\\";
		inline constexpr static const char* SHADERS_DIR = "Shaders\\";
		inline constexpr static const char* FONTS_DIR = "Fonts\\";
		inline constexpr static const char* WORLDS_DIR = "Worlds\\";
		inline constexpr static const char* DATABASES_DIR = "Databases\\";

		namespace Extensions
		{
			inline constexpr static const char* BAKED_SDF_FONT_FILE = ".bpf";
			inline constexpr static const char* WORLD_FILE = ".bpw";
			inline constexpr static const char* WORLD_FILE_NON_OPTIMIZED = ".bpl";
			inline constexpr static const char* FILE_DATABASE = ".bpdb";
			inline constexpr static const char* FILE_DATABASE_INDEX = ".bpdbi";
			inline constexpr static const char* COMPILED_SHADER = ".bpcs";
		}
	}
}