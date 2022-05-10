#include "CodeMacroFinderPCH.h"

int main(int argc, char* argv[])
{
	LeakDetector::Initialize();

	{
		CommandLine::Initialize(argc, argv);

		constexpr static const char* FLAG_MACRO_NAME = "macro";
		constexpr static const char* FLAG_SEARCH_PATH = "path";
		constexpr static const char* FLAG_OUTPUT_FILE = "output";

		if (!CommandLine::IsFlagSet(FLAG_MACRO_NAME) || !CommandLine::IsFlagSet(FLAG_SEARCH_PATH) || !CommandLine::IsFlagSet(FLAG_OUTPUT_FILE))
		{
			LOG_ERROR("Incorrect command line arguments, usage: -macro=\"macro name here\" -path=\"code path here\" -output=\"output file here\"");
			return -1;
		}

		const std::string macroName = CommandLine::GetFlagValue(FLAG_MACRO_NAME);
		const std::string searchPath = CommandLine::GetFlagValue(FLAG_SEARCH_PATH);
		const std::string outputFile = CommandLine::GetFlagValue(FLAG_OUTPUT_FILE);

		if (!std::filesystem::exists(searchPath))
		{
			LOG_ERROR("Search path specified by the -path argument doesn't exist, path=%s", searchPath.c_str());
			return -1;
		}

		// TODO: loop over files in searchPath
	}

	LeakDetector::GetInstance()->DumpLeaks();
	return 0;
}