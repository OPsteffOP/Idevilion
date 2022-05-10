// Folder: Networking

#pragma once

#include "NWinClient.h"
#include "NWinServer.h"

namespace NFactory
{
	inline NClient* CreateClient()
	{
#ifdef BP_PLATFORM_WINDOWS
		return new NWinClient();
#endif
	}

	inline NServer* CreateServer()
	{
#ifdef BP_PLATFORM_WINDOWS
		return new NWinServer();
#endif
	}
}