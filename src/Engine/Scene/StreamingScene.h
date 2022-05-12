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
	void UnloadIdleChunks();

private:
	static constexpr const uint MAX_CHUNK_IDLE_TIME = 5 * 1000;

	WorldStreamer m_WorldStreamer;

	std::mutex m_LoadedChunkIndexesMutex;
	std::unordered_set<Point2i, Point2i::Hasher> m_LoadedChunkIndexes;

	uint m_UnloadIdleChunkTaskIdentifier;
};