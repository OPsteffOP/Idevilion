#include "LoginServerPCH.h"
#include "LoginServer.h"

int main(int argc, char* argv[])
{
	LeakDetector::Initialize();

	{
		CommandLine::Initialize(argc, argv);

		GameTime::Start();

		if (!CommandLine::IsFlagSet("-server_port") || !CommandLine::IsFlagSet("-database_server") || !CommandLine::IsFlagSet("-database_port") || !CommandLine::IsFlagSet("-database_auth_username") || !CommandLine::IsFlagSet("-database_auth_password"))
		{
			LOG_ERROR("Invalid network command line arguments, usage: -server_port=uint here -database_server=\"string here\" -database_port=uint here -database_auth_username=\"string here\" -database_auth_password=\"string here\"");
			return -1;
		}

		const std::string& databaseHost = CommandLine::GetFlagValue("-database_server");
		const uint databasePort = (uint)std::stoi(CommandLine::GetFlagValue("-database_port"));
		const std::string& databaseUsername = CommandLine::GetFlagValue("-database_auth_username");
		const std::string& databasePassword = CommandLine::GetFlagValue("-database_auth_password");

		LoginServer server;
		if (!server.ConnectDatabase(databaseHost, databasePort, databaseUsername, databasePassword))
		{
			LOG_ERROR("Couldn't create a connection to the database server, server=%s:%d", databaseHost.c_str(), databasePort);
			return -1;
		}

		server.Start((uint)std::stoi(CommandLine::GetFlagValue("-server_port")));
	}

	LeakDetector::GetInstance()->DumpLeaks();
}