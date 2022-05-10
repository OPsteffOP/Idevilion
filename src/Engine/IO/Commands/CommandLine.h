// Folder: IO/Commands

#pragma once

class CommandLine
{
public:
	static void Initialize(int argc, char* argv[]);

	static bool IsFlagSet(const std::string& flag);
	static const std::string& GetFlagValue(const std::string& flag);

private:
	static CommandLine* GetInstance();

private:
	static const std::string EMPTY_FLAG_VALUE;
	std::unordered_map<std::string, std::string> m_Data;
};