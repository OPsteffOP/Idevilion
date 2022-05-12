// Folder: Scene

#pragma once
#include "GameObject.h"

class Camera;
class Buffer;

struct Chunk
{
	std::vector<GameObject*> pGameObjects;
	long long lastUpdateTimestamp;
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
	void RemoveGameObject(GameObject* pGameObject, bool shouldDelete = true);

	void RemoveLastGameObjectAtLocation(const Point2f& position, bool shouldDelete = true);

	const std::unordered_map<Point2i, Chunk, Point2i::Hasher>& GetLoadedChunks() const { return m_LoadedChunks; }

public:
	static constexpr Point2i CHUNK_SIZE = Point2i(256, 256);

protected:
	Rect4i GetVisibleChunksRect() const;

protected:
	enum class ActionType
	{
		ADD,
		REMOVE,
		DELETE
	};

	std::mutex m_PendingActionsMutex;
	std::unordered_map<GameObject*, ActionType> m_PendingGameObjectActions;
	std::unordered_map<Point2i, ActionType, Point2i::Hasher> m_PendingChunkActions;

	std::unordered_map<Point2i, Chunk, Point2i::Hasher> m_LoadedChunks;

private:
	void HandlePendingChunkActions();
	void HandlePendingGameObjectActions();
	void ValidateChunks();
	void CleanupDeletedGameObjects();
};