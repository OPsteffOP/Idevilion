// Folder: Scene

#pragma once
#include "GameObject.h"

class Camera;
class Buffer;

struct Chunk
{
	std::vector<GameObject*> pGameObjects;
};

class Scene
{
public:
	Scene() = default;
	virtual ~Scene();

	void Update();
	void Render();
#ifdef DEV_DEBUG_BUILD
	void DebugRender();
#endif

	void AddGameObject(GameObject* pGameObject);
	void RemoveGameObject(GameObject* pGameObject);

	void RemoveLastGameObjectAtLocation(const Point2f& position);

	const std::unordered_map<Point2i, Chunk, Point2i::Hasher>& GetLoadedChunks() const { return m_LoadedChunks; }

public:
	static constexpr Point2i CHUNK_SIZE = Point2i(256, 256);

protected:
	std::unordered_map<Point2i, Chunk, Point2i::Hasher> m_LoadedChunks;

private:
	void ValidateChunks();
	void CleanupDeletedGameObjects();
};