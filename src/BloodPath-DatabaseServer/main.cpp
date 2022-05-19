#include "DatabaseServerPCH.h"

#include "Databases.h"
#include "DatabaseServer.h"

int main(int argc, char* argv[])
{
	LeakDetector::Initialize();

	{
		CommandLine::Initialize(argc, argv);

		GameTime::Start();

		if (!CommandLine::IsFlagSet("-server_port"))
		{
			LOG_ERROR("Invalid network command line arguments, usage: -server_port=uint here");
			return -1;
		}

		if (!CommandLine::IsFlagSet("-server_database") || !CommandLine::IsFlagSet("-user_database"))
		{
			LOG_ERROR("Invalid I/O command line arguments, usage: -server_database=\"string here\" -user_database=\"string here\"");
			return -1;
		}

		Paths::GAME_NAME = "Idevilion";
		Paths::Data::CORE_DIR = Utils::ForceEndWithPathSeparator(std::filesystem::path(Paths::OS::GetLocalAppFolder()).append(Paths::COMPANY_NAME + "/" + Paths::GAME_NAME).string());
		Paths::Data::SHADER_CACHE_DIR = Utils::ForceEndWithPathSeparator(Paths::Data::CORE_DIR + "shader_cache");
		Paths::Data::DATA_DIR = Utils::ForceEndWithPathSeparator(Paths::Data::CORE_DIR + "data-server");

		Databases::pServerDatabase = new ServerDatabase(Paths::Data::DATA_DIR + Paths::Data::DATABASES_DIR + CommandLine::GetFlagValue("-server_database") + Paths::Data::Extensions::FILE_DATABASE);
		Databases::pUserDatabase = new UserDatabase(Paths::Data::DATA_DIR + Paths::Data::DATABASES_DIR + CommandLine::GetFlagValue("-user_database") + Paths::Data::Extensions::FILE_DATABASE);

		DatabaseServer server;
		server.Start((uint)std::stoi(CommandLine::GetFlagValue("-server_port")));

		Databases::ReleaseDatabases();
	}

	LeakDetector::GetInstance()->DumpLeaks();
}