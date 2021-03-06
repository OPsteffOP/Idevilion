// Folder: Scene

#include "EnginePCH.h"
#include "Scene.h"

#include "CameraManager.h"
#include "GameObject.h"
#include "Camera.h"

Scene::~Scene()
{
	for (std::pair<const Point2i, Chunk>& pair : m_LoadedChunks)
	{
		for (GameObject* pGameObject : pair.second.pGameObjects)
		{
			SAFE_DELETE(pGameObject);
		}
	}
}

void Scene::Update()
{
	Rect4i visibleChunksRect(INT_MIN / 2, INT_MIN / 2, INT_MAX, INT_MAX); // if there's no camera, just update & render everything
	if (CameraManager::GetInstance()->GetActiveMainCamera() != nullptr)
	{
		visibleChunksRect = GetVisibleChunksRect();
		visibleChunksRect.x -= 1; // adding an extra chunk on each side for the update to avoid moving objects suddenly stop moving and not coming in view anymore
		visibleChunksRect.y -= 1;
		visibleChunksRect.width += 1;
		visibleChunksRect.height += 1;
	}

	for (std::pair<const Point2i, Chunk>& pair : m_LoadedChunks)
	{
		if (!visibleChunksRect.IsInside(pair.first))
			continue;

		pair.second.lastUpdateTimestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

		for (GameObject* pGameObject : pair.second.pGameObjects)
		{
			if (pGameObject != nullptr)
				pGameObject->Update();
		}
	}

	HandlePendingChunkActions();
	HandlePendingGameObjectActions();
	ValidateChunks();
	CleanupDeletedGameObjects();
}

void Scene::Render()
{
	const Rect4i visibleChunksRect = GetVisibleChunksRect();

	for (const std::pair<Point2i, Chunk>& pair : m_LoadedChunks)
	{
		if (!visibleChunksRect.IsInside(pair.first))
			continue;

		for (GameObject* pGameObject : pair.second.pGameObjects)
		{
			if (pGameObject != nullptr)
				pGameObject->Render();
		}
	}
}

#ifdef DEV_DEBUG_BUILD
#include "GraphicsFactory.h"
void Scene::DebugRender()
{
	static Buffer* pVertexBuffer = nullptr;
	static ShaderState* pVertexShader = nullptr;
	static ShaderState* pPixelShader = nullptr;
	static InputLayout* pInputLayout = nullptr;
	if (pVertexBuffer == nullptr)
	{
		ALLOW_NEXT_ALLOCATION_LEAK();
		pVertexBuffer = GraphicsFactory::CreateBuffer((uint)sizeof(Point2f), 8, BufferUsage::DYNAMIC,
			ShaderAccessFlags::NO_ACCESS, CPUAccessFlags::WRITE, BindFlags::VERTEX_BUFFER);

		ALLOW_NEXT_ALLOCATION_LEAK();
		pVertexShader = GraphicsFactory::CreateShaderState(ShaderType::VERTEX_SHADER, Paths::Data::DATA_DIR + Paths::Data::SHADERS_DIR + "LineWorldSpaceVS.hlsl");
		ALLOW_NEXT_ALLOCATION_LEAK();
		pPixelShader = GraphicsFactory::CreateShaderState(ShaderType::PIXEL_SHADER, Paths::Data::DATA_DIR + Paths::Data::SHADERS_DIR + "DefaultLinePS.hlsl");

		std::vector<InputElement> inputElements;
		inputElements.push_back(InputElement{ "POSITION", 0, PixelFormat::R32G32_FLOAT, InputElementType::VERTEX_DATA });
		ALLOW_NEXT_ALLOCATION_LEAK();
		pInputLayout = GraphicsFactory::CreateInputLayout(pVertexShader, inputElements);
	}

	Camera* pCamera = CameraManager::GetInstance()->GetActiveMainCamera();
	const Rect4f worldRectInView = pCamera->GetWorldRectangleInView();

	const uint chunksX = (uint)std::ceilf(worldRectInView.width / CHUNK_SIZE.x) + 1;
	const uint chunksY = (uint)std::ceilf(worldRectInView.height / CHUNK_SIZE.y) + 1;

	const Point2i baseChunkIndex = Point2i((int)std::floor(worldRectInView.x / (float)CHUNK_SIZE.x), (int)std::floor(worldRectInView.y / (float)CHUNK_SIZE.y));

	const Rect4f chunkLoadingRect((float)baseChunkIndex.x * CHUNK_SIZE.x, (float)baseChunkIndex.y * CHUNK_SIZE.y, (float)chunksX * CHUNK_SIZE.x, (float)chunksY * CHUNK_SIZE.y);

	Point2f vertices[8]
	{
		Point2f{ chunkLoadingRect.x, chunkLoadingRect.y }, Point2f{ chunkLoadingRect.x + chunkLoadingRect.width, chunkLoadingRect.y },
		Point2f{ chunkLoadingRect.x + chunkLoadingRect.width, chunkLoadingRect.y }, Point2f{ chunkLoadingRect.x + chunkLoadingRect.width, chunkLoadingRect.y + chunkLoadingRect.height },
		Point2f{ chunkLoadingRect.x + chunkLoadingRect.width, chunkLoadingRect.y + chunkLoadingRect.height }, Point2f{ chunkLoadingRect.x, chunkLoadingRect.y + chunkLoadingRect.height },
		Point2f{ chunkLoadingRect.x, chunkLoadingRect.y + chunkLoadingRect.height }, Point2f{ chunkLoadingRect.x, chunkLoadingRect.y }
	};
	pVertexBuffer->UpdateData(vertices);

	Renderer* pRenderer = Renderer::GetRenderer();
	pRenderer->SetPrimitiveTopology(PrimitiveTopology::LINES);

	pRenderer->BindVertexBuffer(pVertexBuffer);
	pRenderer->BindShaderState(pVertexShader);
	pRenderer->BindShaderState(pPixelShader);
	pRenderer->BindInputLayout(pInputLayout);

	pRenderer->Draw(8);

	pRenderer->SetPrimitiveTopology(PrimitiveTopology::TRIANGLES);
}
#endif

void Scene::AddGameObject(GameObject* pGameObject)
{
	std::unique_lock lock(m_PendingActionsMutex);
	m_PendingGameObjectActions.emplace(pGameObject, ActionType::ADD);
}

void Scene::RemoveGameObject(GameObject* pGameObject, bool shouldDelete)
{
	std::unique_lock lock(m_PendingActionsMutex);

	const ActionType actionType = shouldDelete ? ActionType::DELETE : ActionType::REMOVE;
	m_PendingGameObjectActions.emplace(pGameObject, actionType);
}

void Scene::RemoveLastGameObjectAtLocation(const Point2f& position, bool shouldDelete)
{
	const Point2i chunkIndex = Point2i((int)std::floor(position.x / (float)CHUNK_SIZE.x), (int)std::floor(position.y / (float)CHUNK_SIZE.y));

	if (m_LoadedChunks.find(chunkIndex) == m_LoadedChunks.end())
		return;

	std::vector<GameObject*>& pGameObjects = m_LoadedChunks[chunkIndex].pGameObjects;

	std::vector<GameObject*>::iterator foundIterator = std::find_if(pGameObjects.begin(), pGameObjects.end(),
		[&position](GameObject* pGameObject) { return pGameObject != nullptr && pGameObject->GetPosition() == position; });
	if (foundIterator != pGameObjects.end())
	{
		std::unique_lock lock(m_PendingActionsMutex);

		const ActionType actionType = shouldDelete ? ActionType::DELETE : ActionType::REMOVE;
		m_PendingGameObjectActions.emplace(*foundIterator, actionType);
	}
}

void Scene::HandlePendingChunkActions()
{
	std::unique_lock lock(m_PendingActionsMutex);

	for (const std::pair<Point2i, ActionType>& pair : m_PendingChunkActions)
	{
		if (pair.second == ActionType::ADD)
		{
			m_LoadedChunks[pair.first].lastUpdateTimestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		}
		else
		{
			for (GameObject* pGameObject : m_LoadedChunks[pair.first].pGameObjects)
			{
				m_PendingGameObjectActions[pGameObject] = pair.second;
			}

			m_LoadedChunks.erase(pair.first);
		}
	}
	m_PendingChunkActions.clear();
}

void Scene::HandlePendingGameObjectActions()
{
	std::unique_lock lock(m_PendingActionsMutex);

	for (const std::pair<GameObject*, ActionType>& pair : m_PendingGameObjectActions)
	{
		GameObject* pGameObject = pair.first;

		if (pair.second == ActionType::ADD)
		{
			const Point2i chunkIndex = Point2i((int)std::floor(pGameObject->GetPosition().x / (float)CHUNK_SIZE.x), (int)std::floor(pGameObject->GetPosition().y / (float)CHUNK_SIZE.y));
			m_LoadedChunks[chunkIndex].pGameObjects.push_back(pGameObject);

			m_LoadedChunks[chunkIndex].lastUpdateTimestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

			pGameObject->m_pParentScene = this;
			pGameObject->Enable();
		}
		else
		{
			pGameObject->m_pParentScene = nullptr;
			pGameObject->Disable();

			const Point2i chunkIndex = Point2i((int)std::floor(pGameObject->GetPosition().x / (float)CHUNK_SIZE.x), (int)std::floor(pGameObject->GetPosition().y / (float)CHUNK_SIZE.y));
			if (m_LoadedChunks.find(chunkIndex) != m_LoadedChunks.end())
			{
				std::vector<GameObject*>& pChunkGameObjects = m_LoadedChunks[chunkIndex].pGameObjects;
				pChunkGameObjects.erase(std::remove(pChunkGameObjects.begin(), pChunkGameObjects.end(), pGameObject), pChunkGameObjects.end());

				m_LoadedChunks[chunkIndex].lastUpdateTimestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
			}

			if (pair.second == ActionType::DELETE)
				delete pGameObject;
		}
	}
	m_PendingGameObjectActions.clear();
}

void Scene::ValidateChunks()
{
	std::vector<GameObject*> pMovedChunkObjects;

	for (std::pair<const Point2i, Chunk>& pair : m_LoadedChunks)
	{
		const Rect4f chunkRect((float)pair.first.x * CHUNK_SIZE.x, (float)pair.first.y * CHUNK_SIZE.y, (float)CHUNK_SIZE.x, (float)CHUNK_SIZE.y);

		std::vector<GameObject*>& pGameObjects = pair.second.pGameObjects;
		pGameObjects.erase(std::remove_if(pGameObjects.begin(), pGameObjects.end(), [&pMovedChunkObjects, &chunkRect](GameObject* pGameObject)
			{
				if (pGameObject != nullptr && !chunkRect.IsInside(pGameObject->GetPosition()))
				{
					pMovedChunkObjects.push_back(pGameObject);
					return true;
				}

				return false;
			}), pGameObjects.end());
	}

	for (GameObject* pGameObject : pMovedChunkObjects)
	{
		const Point2i chunkIndex = Point2i((int)std::floor(pGameObject->GetPosition().x / (float)CHUNK_SIZE.x), (int)std::floor(pGameObject->GetPosition().y / (float)CHUNK_SIZE.y));
		m_LoadedChunks[chunkIndex].pGameObjects.push_back(pGameObject);
		m_LoadedChunks[chunkIndex].lastUpdateTimestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	}
}

void Scene::CleanupDeletedGameObjects()
{
	for (std::pair<const Point2i, Chunk>& pair : m_LoadedChunks)
	{
		std::vector<GameObject*>& pGameObjects = pair.second.pGameObjects;
		pGameObjects.erase(std::remove(pGameObjects.begin(), pGameObjects.end(), nullptr), pGameObjects.end());
	}
}

Rect4i Scene::GetVisibleChunksRect() const
{
	const Camera* pCamera = CameraManager::GetInstance()->GetActiveMainCamera();
	const Rect4f worldRectInView = pCamera->GetWorldRectangleInView();

	const int baseChunkIndexX = (int)std::floor(worldRectInView.x / (float)CHUNK_SIZE.x);
	const int baseChunkIndexY = (int)std::floor(worldRectInView.y / (float)CHUNK_SIZE.y);
	const int chunksCountX = (int)std::ceilf(worldRectInView.width / CHUNK_SIZE.x) + 1;
	const int chunksCountY = (int)std::ceilf(worldRectInView.height / CHUNK_SIZE.y) + 1;

	return Rect4i(baseChunkIndexX, baseChunkIndexY, chunksCountX, chunksCountY);
}