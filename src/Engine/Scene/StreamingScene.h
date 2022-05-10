// Folder: Scene

#pragma once
#include "Scene.h"
#include "WorldStreamer.h"

class StreamingScene : public Scene
{
public:
	explicit StreamingScene(const std::string& worldFilePath);

	void UpdateChunkLoading(Camera* pCamera);

private:
	WorldStreamer m_WorldStreamer;
};