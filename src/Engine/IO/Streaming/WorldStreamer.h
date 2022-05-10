// Folder: IO/Streaming

#pragma once
#include "ByteStream.h"

class GameObject;
class Scene;

class WorldStreamer
{
public:
	explicit WorldStreamer(const std::string& worldFilePath);

	void LoadHeaders();

	void LoadChunk_Client(const Point2i& chunkIndex, Scene* pScene);
	void LoadChunk_Server(const Point2i& chunkIndex, Scene* pScene);

private:
	ByteStream m_WorldFileStream;
	std::unordered_map<Point2i, uint, Point2i::Hasher> m_ChunkFilePositions;
};