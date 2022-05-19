#include "BloodPathPCH.h"

#ifdef STEAM_BUILD
#include "SteamModule.h"
#endif

#ifdef BP_PLATFORM_ANDROID
#include <android\native_activity.h>
#endif

#include "InitializationState.h"
#include "LoginScreen.h"
#include "ClientGame.h"

#include "ByteStream.h"

int UniversalEntryPoint(int argc, char* argv[], void* pWindowUserData)
{
	LeakDetector::Initialize();

	{
		CommandLine::Initialize(argc, argv);

#ifdef STEAM_BUILD
		if (!SteamModule::InitializeSteam(0))
			return -1;
#endif

		GameTime::Start();

		// Initialize the window, renderer, and core graphics objects
		bool shouldContinue = false;
		{
			shouldContinue = InitializationState::Initialize(pWindowUserData);
		}

		if (shouldContinue)
		{
			EngineData engineInitializeData;
			engineInitializeData.gameName = "Idevilion";
			engineInitializeData.coreFolder = std::filesystem::path(Paths::OS::GetLocalAppFolder()).append(Paths::COMPANY_NAME + "/" + engineInitializeData.gameName).string();
			engineInitializeData.shaderCacheFolder = "shader_cache";
			engineInitializeData.dataFolder = "data";

			std::string worldServerIP;
			uint worldServerPort;
			NetworkUUID sessionToken;
			{
				LoginScreen loginScreen;
				loginScreen.Start(engineInitializeData);

				worldServerIP = loginScreen.GetWorldServerIP();
				worldServerPort = loginScreen.GetWorldServerPort();
				sessionToken = loginScreen.GetSessionToken();
			}

			{
				ClientGame game(worldServerIP, worldServerPort, sessionToken);
				game.Start(engineInitializeData);
			}
		}

#ifdef STEAM_BUILD
		SteamModule::ShutdownSteam();
#endif
		}

	LeakDetector::GetInstance()->DumpLeaks();
	return 0;
	}

#ifdef BP_PLATFORM_WINDOWS
int main(int argc, char* argv[])
{
	return UniversalEntryPoint(argc, argv, nullptr);
}
#endif

#ifdef BP_PLATFORM_ANDROID
void ANativeActivity_onCreate(ANativeActivity* pActivity, void* pSavedState, size_t savedStateSize)
{
	char* args[1]{ "" };
	UniversalEntryPoint(1, args, pActivity);
}
#endif