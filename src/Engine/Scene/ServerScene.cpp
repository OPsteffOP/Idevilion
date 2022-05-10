// Folder: Scene

#include "EnginePCH.h"
#include "ServerScene.h"

ServerScene::ServerScene(const std::string& worldFilePath, const Point2i& bottomLeftPos, const Point2i& topRightPos)
	: m_WorldStreamer(worldFilePath)
{
	for (int y = bottomLeftPos.y; y <= topRightPos.y; ++y)
	{
		for (int x = bottomLeftPos.x; x <= topRightPos.x; ++x)
		{
			const Point2i chunkIndex = Point2i(x, y);
			m_WorldStreamer.LoadChunk_Server(chunkIndex, this);
		}
	}
}