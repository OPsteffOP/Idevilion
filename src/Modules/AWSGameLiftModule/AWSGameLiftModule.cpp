#include "AWSGameLiftModulePCH.h"
#include "AWSGameLiftModule.h"

#include <core/Aws.h>
#include <gamelift\GameLiftClient.h>

MODULE_API
void AWSGameLiftModule::InitializeGameLift()
{
	Aws::SDKOptions options;
	Aws::InitAPI(options);
}

MODULE_API
void AWSGameLiftModule::ShutdownGameLift()
{
	Aws::SDKOptions options;
	Aws::ShutdownAPI(options);
}