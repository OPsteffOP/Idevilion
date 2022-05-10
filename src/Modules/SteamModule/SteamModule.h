#pragma once

namespace SteamModule
{
	MODULE_API bool InitializeSteam(uint appID);
	MODULE_API void ShutdownSteam();
}