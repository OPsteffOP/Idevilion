// Folder: Game

#include "EnginePCH.h"
#include "Game.h"

#include "WindowManager.h"

Game::Game()
	: m_IsShuttingDown(false)
{}

void Game::Start()
{
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