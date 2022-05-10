// Folder: Utils

#include "EnginePCH.h"
#include "StringUtils.h"

#include <codecvt>
#include <locale>

std::wstring Utils::StringToWideString(const std::string& str)
{
#pragma warning(push)
#pragma warning(disable: 4996)
	return std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(str);
#pragma warning(pop)
}

std::string Utils::WideStringToString(const std::wstring& str)
{
#pragma warning(push)
#pragma warning(disable: 4996)
	return std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(str);
#pragma warning(pop)
}