// Folder: IO/Paths

#pragma once

namespace Paths
{
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
		inline static std::string DATA_DIR = /*std::filesystem::path(Paths::OS::GetLocalAppFolder()).append("VeryCoolEngine\\data\\").string();*/ "C:\\Users\\Administrator\\Perforce\\STEFF-DESKTOP\\P3-Placeholder\\Dev\\data\\";
		inline static std::string DATA_SERVER_DIR = /*std::filesystem::path(Paths::OS::GetLocalAppFolder()).append("VeryCoolEngine\\data-server\\").string();*/ "C:\\Users\\Administrator\\Perforce\\STEFF-DESKTOP\\P3-Placeholder\\Dev\\data-server\\";

#ifdef DEV_BUILD
		inline static std::string DATA_DEV_DIR = /*std::filesystem::path(Paths::OS::GetLocalAppFolder()).append("VeryCoolEngine\\data-dev\\").string();*/ "C:\\Users\\Administrator\\Perforce\\STEFF-DESKTOP\\P3-Placeholder\\Dev\\data-dev\\";
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
		}
	}
}