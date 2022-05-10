#include "WorldServerPCH.h"

#include "ServerGame.h"

int main(int argc, char* argv[])
{
	LeakDetector::Initialize();

	{
		CommandLine::Initialize(argc, argv);

		ServerGame server;
		server.Start();
	}

	LeakDetector::GetInstance()->DumpLeaks();
}