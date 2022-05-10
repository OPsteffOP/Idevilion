#include "SteamModulePCH.h"
#include "SteamModule.h"

#pragma warning(push)
#pragma warning(disable: 4996)
#include <steam_api.h>
#pragma warning(pop)

MODULE_API
bool SteamModule::InitializeSteam(uint appID)
{
	if (SteamAPI_RestartAppIfNecessary(appID))
	{
		LOG_DEBUG("Game wasn't launched through steam, relaunching the game through steam");
		return false;
	}

	if (!SteamAPI_Init())
	{
		LOG_ERROR("Steam must be running to play this game!");
		return false;
	}

	return true;
}

MODULE_API
void SteamModule::ShutdownSteam()
{
	SteamAPI_Shutdown();
}