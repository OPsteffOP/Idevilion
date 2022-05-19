// Folder: Game

#include "EnginePCH.h"
#include "Game.h"

#include "WindowManager.h"

Game::Game()
	: m_IsShuttingDown(false)
{}

void Game::Start(const EngineData& data)
{
	Paths::GAME_NAME = data.gameName;
	Paths::Data::CORE_DIR = Utils::ForceEndWithPathSeparator(data.coreFolder);
	Paths::Data::SHADER_CACHE_DIR = Utils::ForceEndWithPathSeparator(Paths::Data::CORE_DIR + data.shaderCacheFolder);
	Paths::Data::DATA_DIR = Utils::ForceEndWithPathSeparator(Paths::Data::CORE_DIR + data.dataFolder);

	Initialize();

	while (!m_IsShuttingDown)
	{
		Update();
		Render();

		Window* pMainWindow = WindowManager::GetInstance()->GetWindow(WindowIdentifier::MAIN_WINDOW);
		if (pMainWindow != nullptr)
		{
			if (pMainWindow->IsClosed())
				m_IsShuttingDown = true;
		}
	}
}

void Game::Shutdown()
{
	m_IsShuttingDown = true;
}