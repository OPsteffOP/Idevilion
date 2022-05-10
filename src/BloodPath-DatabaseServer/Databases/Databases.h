// Folder: Databases

#pragma once
#include "ServerDatabase.h"
#include "UserDatabase.h"

namespace Databases
{
	inline ServerDatabase* pServerDatabase;
	inline UserDatabase* pUserDatabase;

	inline void ReleaseDatabases()
	{
		SAFE_DELETE(pServerDatabase);
		SAFE_DELETE(pUserDatabase);
	}
}