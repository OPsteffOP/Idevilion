// Folder: IO/Streaming

#include "EnginePCH.h"
#include "WorldStreamer.h"

#include "PrefabFactory.h"
#include "Scene.h"

WorldStreamer::WorldStreamer(const std::string& worldFilePath)
	: m_WorldFileStream(new ByteStreamBinaryFileImpl(worldFilePath))
{
	if (!std::filesystem::exists(worldFilePath))
		LOG_ERROR("Failed to initialize world streamer with world_file=%s", worldFilePath.c_str());

	LoadHeaders();
}

void WorldStreamer::LoadHeaders()
{
	m_WorldFileStream.SetReadBufferPointer(0);

	const uint chunksCount = m_WorldFileStream.ReadUInt32();

	m_ChunkFilePositions.reserve(chunksCount);
	for (uint i = 0; i < chunksCount; ++i)
	{
		const Point2i chunkIndex = m_WorldFileStream.ReadPoint2i();
		const uint chunkFilePosition = m_WorldFileStream.ReadUInt32();

		m_ChunkFilePositions.emplace(chunkIndex, chunkFilePosition);
	}
}

void WorldStreamer::LoadChunk_Client(const Point2i& chunkIndex, Scene* pScene)
{
	if (m_ChunkFilePositions.find(chunkIndex) == m_ChunkFilePositions.end())
		return;

	const Point2f destPosition = chunkIndex * Scene::CHUNK_SIZE;

	Rect4f destRect;
	destRect.x = destPosition.x;
	destRect.y = destPosition.y;
	destRect.width = (float)Scene::CHUNK_SIZE.x;
	destRect.height = (float)Scene::CHUNK_SIZE.y;

	//constexpr uint maxStringLength = 1024;
	//char stringBuffer[maxStringLength];

	const uint chunkFilePosition = m_ChunkFilePositions[chunkIndex];
	m_WorldFileStream.SetReadBufferPointer(chunkFilePosition);

	// Reading and creating static background
	const uint backgroundPathLength = m_WorldFileStream.ReadUInt32();
	const std::string backgroundPath = m_WorldFileStream.ReadString(backgroundPathLength);
	pScene->AddGameObject(PrefabFactory::CreateTextureRenderObject(Paths::Data::DATA_DIR + backgroundPath, destRect));

	// TODO: load all GameObjects with Components in the chunk
	//game object prefabs :
	//-make a map : std::unordered_map<uint, CreatePrefabFunc>
	//	where uint is the entity id
	//	and CreatePrefabFunc is a function with signature GameObject * (const Point2f & pos) that returns the prefab with all attached components

	//		then the world file can just store the entity id and position

	// Reading and creating static foreground
	const uint foregroundPathLength = m_WorldFileStream.ReadUInt32();
	const std::string foregroundPath = m_WorldFileStream.ReadString(foregroundPathLength);
	pScene->AddGameObject(PrefabFactory::CreateTextureRenderObject(Paths::Data::DATA_DIR + foregroundPath, destRect));
}

void WorldStreamer::LoadChunk_Server(const Point2i& chunkIndex, Scene* pScene)
{
	if (m_ChunkFilePositions.find(chunkIndex) == m_ChunkFilePositions.end())
		return;

	const Point2f destPosition = chunkIndex * Scene::CHUNK_SIZE;

	Rect4f destRect;
	destRect.x = destPosition.x;
	destRect.y = destPosition.y;
	destRect.width = (float)Scene::CHUNK_SIZE.x;
	destRect.height = (float)Scene::CHUNK_SIZE.y;

	const uint chunkFilePosition = m_ChunkFilePositions[chunkIndex];
	m_WorldFileStream.SetReadBufferPointer(chunkFilePosition);

	// TODO: load all server objects (colliders, NPCs, ...)
	// and add to pScene
	UNREFERENCED_PARAMETER(pScene);
}