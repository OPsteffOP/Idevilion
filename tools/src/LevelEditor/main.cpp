#include "LevelEditorPCH.h"

#include "LevelEditor.h"

int main(int argc, char* argv[])
{
	LeakDetector::Initialize();

	{
		CommandLine::Initialize(argc, argv);

		EngineData engineInitializeData;
		engineInitializeData.gameName = "Idevilion";
		engineInitializeData.coreFolder = std::filesystem::path(Paths::OS::GetLocalAppFolder()).append(Paths::COMPANY_NAME + "/" + engineInitializeData.gameName).string();
		engineInitializeData.shaderCacheFolder = "shader_cache";
		engineInitializeData.dataFolder = "data";

		LevelEditor levelEditor;
		levelEditor.Start(engineInitializeData);
	}

	LeakDetector::GetInstance()->DumpLeaks();
}