// Folder: Scene

#include "EnginePCH.h"
#include "StreamingScene.h"

#include "Camera.h"

StreamingScene::StreamingScene(const std::string& worldFilePath)
	: m_WorldStreamer(worldFilePath)
{}

void StreamingScene::UpdateChunkLoading(Camera* pCamera)
{
	if (pCamera == nullptr)
		return;

	const Rect4f worldRectInView = pCamera->GetWorldRectangleInView();

	const uint chunksCountX = (uint)std::ceilf(worldRectInView.width / CHUNK_SIZE.x) + 1;
	const uint chunksCountY = (uint)std::ceilf(worldRectInView.height / CHUNK_SIZE.y) + 1;

	const Point2i baseChunkIndex = Point2i((int)std::floor(worldRectInView.x / (float)CHUNK_SIZE.x), (int)std::floor(worldRectInView.y / (float)CHUNK_SIZE.y));
	for (uint chunkOffsetY = 0; chunkOffsetY < chunksCountY; ++chunkOffsetY)
	{
		for (uint chunkOffsetX = 0; chunkOffsetX < chunksCountX; ++chunkOffsetX)
		{
			const Point2i chunkIndex = Point2i(baseChunkIndex.x + chunkOffsetX, baseChunkIndex.y + chunkOffsetY);
			AttemptLoadChunk(chunkIndex);
		}
	}

	// if one of the neighbour chunks do not exist, load them on a different thread
	// loading can't really happen on a seperate thread right now because of m_WorldStreamer.LoadChunk_Client reading from the world file and changing the read pointer
}

void StreamingScene::AttemptLoadChunk(const Point2i& chunkIndex)
{
	{
		std::unique_lock lock(m_AddLoadedChunksMutex);

		if (m_LoadedChunks.find(chunkIndex) != m_LoadedChunks.end())
			return;

		if (m_WorldStreamer.IsChunkLoadable(chunkIndex))
			m_LoadedChunks[chunkIndex]; // already adding the chunk to avoid 2 threads loading the same chunk at the same time
	}

	m_WorldStreamer.LoadChunk_Client(chunkIndex, this);
}