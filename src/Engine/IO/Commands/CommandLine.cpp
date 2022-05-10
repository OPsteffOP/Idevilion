// Folder: IO/Commands

#include "EnginePCH.h"
#include "CommandLine.h"

const std::string CommandLine::EMPTY_FLAG_VALUE = "";

CommandLine* CommandLine::GetInstance()
{
	static CommandLine* pInstance = nullptr;
	if (pInstance == nullptr)
	{
		ALLOW_NEXT_ALLOCATION_LEAK();
		pInstance = new CommandLine();
	}

	return pInstance;
}

void CommandLine::Initialize(int argc, char* argv[])
{
	for (int i = 1; i < argc; ++i)
	{
		std::string flag = argv[i];
		std::string value = "";

		const size_t equalSignPosition = flag.find('=');
		if (equalSignPosition != std::string::npos)
		{
			value = flag.substr(equalSignPosition + 1);
			flag = flag.substr(0, equalSignPosition);
		}

#ifndef DEV_BUILD
		// Flags starting with '--' are DEV_BUILD only
		if (flag.size() >= 2 && flag[0] == '-' && flag[1] == '-')
			continue;
#endif

		GetInstance()->m_Data.emplace(flag, value);
	}
}

bool CommandLine::IsFlagSet(const std::string& flag)
{
	return GetInstance()->m_Data.find(flag) != GetInstance()->m_Data.end();
}

const std::string& CommandLine::GetFlagValue(const std::string& flag)
{
	if (IsFlagSet(flag))
		return GetInstance()->m_Data[flag];

	return EMPTY_FLAG_VALUE;
}