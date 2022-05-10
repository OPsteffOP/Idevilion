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

	const uint chunksX = (uint)std::ceilf(worldRectInView.width / CHUNK_SIZE.x) + 1;
	const uint chunksY = (uint)std::ceilf(worldRectInView.height / CHUNK_SIZE.y) + 1;

	const Point2i baseChunkIndex = Point2i((int)std::floor(worldRectInView.x / (float)CHUNK_SIZE.x), (int)std::floor(worldRectInView.y / (float)CHUNK_SIZE.y));
	for (uint chunkOffsetY = 0; chunkOffsetY < chunksY; ++chunkOffsetY)
	{
		for (uint chunkOffsetX = 0; chunkOffsetX < chunksX; ++chunkOffsetX)
		{
			const Point2i chunkIndex = Point2i(baseChunkIndex.x + chunkOffsetX, baseChunkIndex.y + chunkOffsetY);

			if (m_LoadedChunks.find(chunkIndex) != m_LoadedChunks.end())
				continue;

			m_WorldStreamer.LoadChunk_Client(chunkIndex, this);
		}
	}

	// if one of the neighbour chunks do not exist, load them on a different thread
}