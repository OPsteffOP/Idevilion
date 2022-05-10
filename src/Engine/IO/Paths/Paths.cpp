// Folder: IO/Paths

#include "EnginePCH.h"
#include "Paths.h"

#ifdef BP_PLATFORM_WINDOWS
#include <ShlObj.h>

std::string GetWindowsFolderHelper(int csidl)
{
	// https://docs.microsoft.com/en-us/windows/win32/shell/csidl
	TCHAR path[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPath(NULL, csidl, NULL, 0, path)))
		return Utils::WideStringToString(path);

	LOG_ERROR("Windows couldn't find the path for csidl=%d", csidl);
	return "NO PATH FOUND";
}
#endif

std::string Paths::OS::GetLocalAppFolder()
{
#ifdef BP_PLATFORM_WINDOWS
	return GetWindowsFolderHelper(CSIDL_LOCAL_APPDATA);
#endif
}

std::string Paths::OS::GetRoamingAppFolder()
{
#ifdef BP_PLATFORM_WINDOWS
	return GetWindowsFolderHelper(CSIDL_APPDATA);
#endif
}

std::string Paths::OS::GetProgramFilesFolder()
{
#ifdef BP_PLATFORM_WINDOWS
	return GetWindowsFolderHelper(CSIDL_PROGRAM_FILES);
#endif
}

std::string Paths::OS::GetLocalDesktopFolder()
{
#ifdef BP_PLATFORM_WINDOWS
	return GetWindowsFolderHelper(CSIDL_DESKTOPDIRECTORY);
#endif
}

std::string Paths::OS::GetFontsFolder()
{
#ifdef BP_PLATFORM_WINDOWS
	return GetWindowsFolderHelper(CSIDL_FONTS);
#endif
}