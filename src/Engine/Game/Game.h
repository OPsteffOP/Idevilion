// Folder: Game

#pragma once

class Game
{
public:
	Game();
	virtual ~Game() = default;

	void Start();

	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;

	void Shutdown();

private:
	bool m_IsShuttingDown;
};