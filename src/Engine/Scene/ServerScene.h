// Folder: Scene

#pragma once
#include "Scene.h"
#include "WorldStreamer.h"

class ServerScene : public Scene
{
public:
	explicit ServerScene(const std::string& worldFilePath, const Point2i& bottomLeftPos, const Point2i& topRightPos);

private:
	WorldStreamer m_WorldStreamer;
};