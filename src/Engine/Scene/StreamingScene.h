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
	void AttemptLoadChunk(const Point2i& chunkIndex);

private:
	WorldStreamer m_WorldStreamer;
	std::mutex m_AddLoadedChunksMutex;
};