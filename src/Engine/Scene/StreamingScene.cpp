// Folder: Scene

#include "EnginePCH.h"
#include "StreamingScene.h"

#include "Camera.h"

StreamingScene::StreamingScene(const std::string& worldFilePath)
	: m_WorldStreamer(worldFilePath)
	, m_UnloadIdleChunkTaskIdentifier(ThreadPool::INVALID_TASK_IDENTIFIER)
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

	// TODO: if one of the neighbour chunks do not exist, load them on a different thread
	// loading can't really happen on a seperate thread right now because of m_WorldStreamer.LoadChunk_Client reading from the world file and changing the read pointer
	// something else to take into account is what if a tile is scheduled to load on a separate thread, but it now comes into view so it needs to be loaded on the main thread
		// added Cancel to ThreadPool so that should be fine? just need to keep track of all the identifiers

	if (ThreadPool::GetInstance()->IsTaskFinished(m_UnloadIdleChunkTaskIdentifier))
		m_UnloadIdleChunkTaskIdentifier = ThreadPool::GetInstance()->QueueWork([this]() { UnloadIdleChunks(); });
}

void StreamingScene::AttemptLoadChunk(const Point2i& chunkIndex)
{
	{
		std::unique_lock lock(m_LoadedChunkIndexesMutex);

		if (m_LoadedChunkIndexes.find(chunkIndex) != m_LoadedChunkIndexes.end())
			return;

		if (m_WorldStreamer.IsChunkLoadable(chunkIndex))
		{
			m_LoadedChunkIndexes.emplace(chunkIndex);

			std::unique_lock pendingActionLock(m_PendingActionsMutex);
			m_PendingChunkActions.emplace(chunkIndex, ActionType::ADD);
		}
	}

	m_WorldStreamer.LoadChunk_Client(chunkIndex, this);
}

void StreamingScene::UnloadIdleChunks()
{
	// TODO: should probably also add a max amount of loaded chunks, and when going over this threshold, start unloading the most idle chunks

	std::vector<Point2i> idleChunks;

	const long long currentTimestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	for (const std::pair<Point2i, Chunk>& pair : m_LoadedChunks)
	{
		if (currentTimestamp - pair.second.lastUpdateTimestamp > MAX_CHUNK_IDLE_TIME)
			idleChunks.push_back(pair.first);
	}

	{
		std::unique_lock lock(m_LoadedChunkIndexesMutex);
		for (const Point2i& idleChunkIndex : idleChunks)
		{
			m_LoadedChunkIndexes.erase(idleChunkIndex);

			std::unique_lock pendingActionLock(m_PendingActionsMutex);
			m_PendingChunkActions.emplace(idleChunkIndex, ActionType::DELETE);
		}
	}
}