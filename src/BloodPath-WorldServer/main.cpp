#include "WorldServerPCH.h"

#include "ServerGame.h"

int main(int argc, char* argv[])
{
	LeakDetector::Initialize();

	{
		CommandLine::Initialize(argc, argv);

		GameTime::Start();

		EngineData engineInitializeData;
		engineInitializeData.gameName = "Idevilion";
		engineInitializeData.coreFolder = std::filesystem::path(Paths::OS::GetLocalAppFolder()).append(Paths::COMPANY_NAME + "/" + engineInitializeData.gameName).string();
		engineInitializeData.shaderCacheFolder = "shader_cache";
		engineInitializeData.dataFolder = "data-server";

		ServerGame server;
		server.Start(engineInitializeData);
	}

	LeakDetector::GetInstance()->DumpLeaks();
}