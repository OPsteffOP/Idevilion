// Folder: Game

#pragma once

struct EngineData
{
	std::string gameName;
	std::string coreFolder;

	std::string shaderCacheFolder;
	std::string dataFolder;
};

class Game
{
public:
	Game();
	virtual ~Game() = default;

	void Start(const EngineData& data);

	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;

	void Shutdown();

private:
	bool m_IsShuttingDown;
};