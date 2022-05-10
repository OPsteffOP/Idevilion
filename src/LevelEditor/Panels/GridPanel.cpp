// Folder: Panels

#include "LevelEditorPCH.h"
#include "GridPanel.h"

#include "GraphicsFactory.h"
#include "Renderer.h"
#include "Buffer.h"
#include "CameraManager.h"
#include "Scene.h"
#include "InputManager.h"
#include "WindowManager.h"
#include "Camera.h"

GridPanel::GridPanel(const Rect4f& rect)
	: m_Rect(rect)
	, m_TileSize(32.f, 32.f)
	, m_TileOffset(0.f, 0.f)
	, m_IsDirty(false)
	, m_GridCurrentVerticesCount(0)
	, m_GridVertices()
	, m_ChunkCurrentVerticesCount(0)
	, m_ChunkVertices()
	, m_HighlightVertices()
	, m_pExclusiveMouseDevice(nullptr)
	, m_IsWorldMoving(false)
	, m_MouseMovedDuringExclusiveMode(false)
	, m_IsRegionSelected(false)
	, m_DidRegionSelectionChange(false)
	, m_SelectedRegion()
{
	Window* pWindow = WindowManager::GetInstance()->GetCurrentWindow();
	m_Rect.y = pWindow->GetHeight() - m_Rect.y - m_Rect.height;

	m_Viewport.topLeftX = m_Rect.x;
	m_Viewport.topLeftY = m_Rect.y;
	m_Viewport.width = m_Rect.width;
	m_Viewport.height = m_Rect.height;
	m_Viewport.minDepth = 0.f;
	m_Viewport.maxDepth = 1.f;

	m_pGridVertexBuffer = GraphicsFactory::CreateBuffer((uint)sizeof(Vertex), m_GridMaxVerticesCount, BufferUsage::DYNAMIC,
		ShaderAccessFlags::NO_ACCESS, CPUAccessFlags::WRITE, BindFlags::VERTEX_BUFFER);
	m_pChunkVertexBuffer = GraphicsFactory::CreateBuffer((uint)sizeof(Vertex), m_ChunkMaxVerticesCount, BufferUsage::DYNAMIC,
		ShaderAccessFlags::NO_ACCESS, CPUAccessFlags::WRITE, BindFlags::VERTEX_BUFFER);
	m_pHighlightVertexBuffer = GraphicsFactory::CreateBuffer((uint)sizeof(Vertex), m_HighlightVerticesCount, BufferUsage::DYNAMIC,
		ShaderAccessFlags::NO_ACCESS, CPUAccessFlags::WRITE, BindFlags::VERTEX_BUFFER);

	m_pVertexShader = GraphicsFactory::CreateShaderState(ShaderType::VERTEX_SHADER, Paths::Data::DATA_DIR + Paths::Data::SHADERS_DIR + "LevelEditorVS.hlsl");
	m_pGridPixelShader = GraphicsFactory::CreateShaderState(ShaderType::PIXEL_SHADER, Paths::Data::DATA_DIR + Paths::Data::SHADERS_DIR + "LevelEditorPS.hlsl", std::vector<std::string>{ "GRID_COLOR" });
	m_pChunkPixelShader = GraphicsFactory::CreateShaderState(ShaderType::PIXEL_SHADER, Paths::Data::DATA_DIR + Paths::Data::SHADERS_DIR + "LevelEditorPS.hlsl", std::vector<std::string>{ "CHUNK_COLOR" });
	m_pHighlightPixelShader = GraphicsFactory::CreateShaderState(ShaderType::PIXEL_SHADER, Paths::Data::DATA_DIR + Paths::Data::SHADERS_DIR + "LevelEditorPS.hlsl", std::vector<std::string>{ "HIGHLIGHT_COLOR" });

	std::vector<InputElement> inputElements;
	inputElements.push_back(InputElement{ "POSITION", 0, PixelFormat::R32G32_FLOAT, InputElementType::VERTEX_DATA });
	m_pInputLayout = GraphicsFactory::CreateInputLayout(m_pVertexShader, inputElements);
}

GridPanel::~GridPanel()
{
	SAFE_DELETE(m_pGridVertexBuffer);
	SAFE_DELETE(m_pChunkVertexBuffer);
	SAFE_DELETE(m_pHighlightVertexBuffer);
	SAFE_DELETE(m_pVertexShader);
	SAFE_DELETE(m_pGridPixelShader);
	SAFE_DELETE(m_pChunkPixelShader);
	SAFE_DELETE(m_pHighlightPixelShader);
	SAFE_DELETE(m_pInputLayout);
}

void GridPanel::Update()
{
	UpdateInput();

	Camera* pCamera = CameraManager::GetInstance()->GetActiveCamera();
	if (m_IsDirty || pCamera->IsDirty())
	{
		UpdateGrid();
		UpdateChunks();
		UpdateHighlight();
	}

	if (m_DidRegionSelectionChange)
	{
		UpdateHighlight();
		m_DidRegionSelectionChange = false;
	}
}

void GridPanel::UpdateInput()
{
	UpdateInputGridMovement();
	UpdateInputWorldZoom();
	UpdateInputGridZoom();
	UpdateInputTileSelection();
}

void GridPanel::UpdateInputGridMovement()
{
	constexpr static const float GRID_MOVE_MULTIPLIER = 1.f;

	Window* pWindow = WindowManager::GetInstance()->GetCurrentWindow();
	const Point2f clientMousePosition = pWindow->GetLocalMousePosition();

	KeyboardInputDevice* pKeyboardDevice = static_cast<KeyboardInputDevice*>(InputManager::GetInstance()->GetDevice(DeviceType::KEYBOARD));
	MouseInputDevice* pMouseDevice = static_cast<MouseInputDevice*>(InputManager::GetInstance()->GetDevice(DeviceType::MOUSE));

	// Claim exclusive device for grid movement
	if (IsPointInGrid(clientMousePosition))
	{
		if (pMouseDevice->GetPreviousState(InputAction::MOUSE_LEFT_BUTTON) == 0.f && pMouseDevice->GetState(InputAction::MOUSE_LEFT_BUTTON) != 0.f)
		{
			m_pExclusiveMouseDevice = InputManager::GetInstance()->ClaimExclusiveInput(DeviceType::MOUSE);
			m_IsWorldMoving = !pKeyboardDevice->IsKeyDown(InputAction::KEYBOARD_LCONTROL);
		}
	}

	if (m_pExclusiveMouseDevice != nullptr)
	{
		const Point2f deltaMousePosition = pMouseDevice->GetDeltaMousePosition();
		if (std::fabsf(deltaMousePosition.x) > FLT_EPSILON || std::fabsf(deltaMousePosition.y) > FLT_EPSILON)
		{
			m_MouseMovedDuringExclusiveMode = true;

			if (m_IsWorldMoving)
			{
				const Point2f& cameraPosition = CameraManager::GetInstance()->GetActiveCamera()->GetCenterPosition();
				const float cameraScale = CameraManager::GetInstance()->GetActiveCamera()->GetScale();

				const Point2f deltaCameraPosition = Point2f((-deltaMousePosition.x * cameraScale) * GRID_MOVE_MULTIPLIER, (deltaMousePosition.y * cameraScale) * GRID_MOVE_MULTIPLIER);
				const Point2f newCameraPosition = cameraPosition + deltaCameraPosition;

				CameraManager::GetInstance()->GetActiveCamera()->SetCenterPosition(newCameraPosition);
			}
			else
			{
				m_TileOffset.x += deltaMousePosition.x;
				m_TileOffset.y += -deltaMousePosition.y;
				m_IsDirty = true;
			}
		}
	}

	// Grid movement done, release exclusive device
	if (m_pExclusiveMouseDevice != nullptr && m_pExclusiveMouseDevice->GetPreviousState(InputAction::MOUSE_LEFT_BUTTON) != 0.f && m_pExclusiveMouseDevice->GetState(InputAction::MOUSE_LEFT_BUTTON) == 0.f)
	{
		InputManager::GetInstance()->ReleaseExclusiveInput(m_pExclusiveMouseDevice);
		m_pExclusiveMouseDevice = nullptr;

		// Mouse not moved during exclusive mode, select tile, and execute tile click event
		if (!m_MouseMovedDuringExclusiveMode)
		{
			// Select tile
			const Camera* pCamera = CameraManager::GetInstance()->GetActiveCamera();
			const Rect4f& cameraRect = pCamera->GetWorldRectangleInView();

			const Point2f mousePosition = Point2f(clientMousePosition.x, (pWindow->GetHeight() - clientMousePosition.y));

			const Point2f rectMousePosition = Point2f(mousePosition.x - m_Rect.x, mousePosition.y - (pWindow->GetHeight() - m_Rect.y - m_Rect.height));
			const Point2f scaledMousePosition = Point2f(rectMousePosition.x * pCamera->GetScale(), rectMousePosition.y * pCamera->GetScale());

			const Point2f tileOffset = Point2f(std::fmodf(m_TileOffset.x, m_TileSize.x), std::fmodf(m_TileOffset.y, m_TileSize.y));

			Point2f tilePosition = Point2f(cameraRect.x - tileOffset.x + scaledMousePosition.x, cameraRect.y - tileOffset.y + scaledMousePosition.y);
			tilePosition.x -= std::fmodf(tilePosition.x, m_TileSize.x) + ((tilePosition.x < 0.f) ? m_TileSize.x : 0);
			tilePosition.y -= std::fmodf(tilePosition.y, m_TileSize.y) + ((tilePosition.y < 0.f) ? m_TileSize.y : 0);
			tilePosition.x += tileOffset.x;
			tilePosition.y += tileOffset.y;

			m_SelectedRegion.x = tilePosition.x;
			m_SelectedRegion.y = tilePosition.y;
			m_SelectedRegion.width = m_TileSize.x;
			m_SelectedRegion.height = m_TileSize.y;

			m_IsRegionSelected = true;
			m_DidRegionSelectionChange = true;

			// Execute click event
			if (m_TileClickEvent != nullptr)
				m_TileClickEvent();
		}

		m_MouseMovedDuringExclusiveMode = false;
	}
}

void GridPanel::UpdateInputWorldZoom()
{
	constexpr static const float WORLD_ZOOM_MULTIPLIER = 0.25f;

	Window* pWindow = WindowManager::GetInstance()->GetCurrentWindow();
	const Point2f clientMousePosition = pWindow->GetLocalMousePosition();

	// Only zoom when mouse is in the grid
	if (!IsPointInGrid(clientMousePosition))
		return;

	MouseInputDevice* pMouseDevice = static_cast<MouseInputDevice*>(InputManager::GetInstance()->GetDevice(DeviceType::MOUSE));
	KeyboardInputDevice* pKeyboardDevice = static_cast<KeyboardInputDevice*>(InputManager::GetInstance()->GetDevice(DeviceType::KEYBOARD));

	if (!pKeyboardDevice->IsKeyDown(InputAction::KEYBOARD_LCONTROL))
	{
		const float currentScale = CameraManager::GetInstance()->GetActiveCamera()->GetScale();
		const float deltaScale = -pMouseDevice->GetState(InputAction::MOUSE_SCROLL) * WORLD_ZOOM_MULTIPLIER;
		const float newScale = currentScale + deltaScale;

		CameraManager::GetInstance()->GetActiveCamera()->SetScale(newScale);
	}
}

void GridPanel::UpdateInputGridZoom()
{
	Window* pWindow = WindowManager::GetInstance()->GetCurrentWindow();
	const Point2f clientMousePosition = pWindow->GetLocalMousePosition();

	// Only zoom when mouse is in the grid
	if (!IsPointInGrid(clientMousePosition))
		return;

	MouseInputDevice* pMouseDevice = static_cast<MouseInputDevice*>(InputManager::GetInstance()->GetDevice(DeviceType::MOUSE));
	KeyboardInputDevice* pKeyboardDevice = static_cast<KeyboardInputDevice*>(InputManager::GetInstance()->GetDevice(DeviceType::KEYBOARD));

	if (pKeyboardDevice->IsKeyDown(InputAction::KEYBOARD_LCONTROL))
	{
		const uint currentZoom = (uint)std::roundf(Scene::CHUNK_SIZE.x / m_TileSize.x);
		const float newTileSize = (float)Scene::CHUNK_SIZE.x / (currentZoom - pMouseDevice->GetState(InputAction::MOUSE_SCROLL));
		TEMP_SetTileSize(newTileSize, newTileSize);
	}
}

void GridPanel::UpdateInputTileSelection()
{
	KeyboardInputDevice* pKeyboardDevice = static_cast<KeyboardInputDevice*>(InputManager::GetInstance()->GetDevice(DeviceType::KEYBOARD));
	if (pKeyboardDevice->IsKeyUp(InputAction::KEYBOARD_ESCAPE))
		m_IsRegionSelected = false;
}

void GridPanel::UpdateGrid()
{
	Camera* pCamera = CameraManager::GetInstance()->GetActiveCamera();

	const Rect4f cameraRect = pCamera->GetWorldRectangleInView();
	const float xGridOffset = std::fmodf(cameraRect.x - m_TileOffset.x, m_TileSize.x);
	const float yGridOffset = std::fmodf(cameraRect.y - m_TileOffset.y, m_TileSize.y);

	// TODO: don't crash when buffer runs out of space
		// resize buffer?
		// stop drawing grid when it runs out?
	// same for UpdateChunks()

	m_GridCurrentVerticesCount = 0;

	float currentX = -xGridOffset;
	while (currentX < cameraRect.width)
	{
		m_GridVertices[m_GridCurrentVerticesCount] = Vertex{ Point2f(currentX / cameraRect.width, 0.f) };
		m_GridVertices[m_GridCurrentVerticesCount + 1] = Vertex{ Point2f(currentX / cameraRect.width, 1.f) };

		m_GridCurrentVerticesCount += 2;
		currentX += m_TileSize.x;
	}

	float currentY = -yGridOffset;
	while (currentY < cameraRect.height)
	{
		m_GridVertices[m_GridCurrentVerticesCount] = Vertex{ Point2f(0.f, currentY / cameraRect.height) };
		m_GridVertices[m_GridCurrentVerticesCount + 1] = Vertex{ Point2f(1.f, currentY / cameraRect.height) };

		m_GridCurrentVerticesCount += 2;
		currentY += m_TileSize.y;
	}

	m_pGridVertexBuffer->UpdateData(m_GridVertices, m_GridCurrentVerticesCount * sizeof(Vertex));
}

void GridPanel::UpdateChunks()
{
	Camera* pCamera = CameraManager::GetInstance()->GetActiveCamera();

	const Rect4f cameraRect = pCamera->GetWorldRectangleInView();
	const float xChunkOffset = std::fmodf(cameraRect.x, (float)Scene::CHUNK_SIZE.x);
	const float yChunkOffset = std::fmodf(cameraRect.y, (float)Scene::CHUNK_SIZE.x);

	m_ChunkCurrentVerticesCount = 0;

	float currentX = -xChunkOffset;
	while (currentX < cameraRect.width)
	{
		m_ChunkVertices[m_ChunkCurrentVerticesCount] = Vertex{ Point2f(currentX / cameraRect.width, 0.f) };
		m_ChunkVertices[m_ChunkCurrentVerticesCount + 1] = Vertex{ Point2f(currentX / cameraRect.width, 1.f) };

		m_ChunkCurrentVerticesCount += 2;
		currentX += (float)Scene::CHUNK_SIZE.x;
	}

	float currentY = -yChunkOffset;
	while (currentY < cameraRect.height)
	{
		m_ChunkVertices[m_ChunkCurrentVerticesCount] = Vertex{ Point2f(0.f, currentY / cameraRect.height) };
		m_ChunkVertices[m_ChunkCurrentVerticesCount + 1] = Vertex{ Point2f(1.f, currentY / cameraRect.height) };

		m_ChunkCurrentVerticesCount += 2;
		currentY += (float)Scene::CHUNK_SIZE.y;
	}

	m_pChunkVertexBuffer->UpdateData(m_ChunkVertices, m_ChunkCurrentVerticesCount * sizeof(Vertex));
}

void GridPanel::UpdateHighlight()
{
	Camera* pCamera = CameraManager::GetInstance()->GetActiveCamera();
	const Rect4f cameraRect = pCamera->GetWorldRectangleInView();

	const Point2f tilePosition = Point2f(m_SelectedRegion.x - cameraRect.x, m_SelectedRegion.y - cameraRect.y);

	m_HighlightVertices[0] = Vertex{ Point2f(tilePosition.x / cameraRect.width, tilePosition.y / cameraRect.height) };
	m_HighlightVertices[1] = Vertex{ Point2f((tilePosition.x + m_SelectedRegion.width) / cameraRect.width, tilePosition.y / cameraRect.height) };
	m_HighlightVertices[2] = m_HighlightVertices[1];
	m_HighlightVertices[3] = Vertex{ Point2f((tilePosition.x + m_SelectedRegion.width) / cameraRect.width, (tilePosition.y + m_SelectedRegion.height) / cameraRect.height) };
	m_HighlightVertices[4] = m_HighlightVertices[3];
	m_HighlightVertices[5] = Vertex{ Point2f(tilePosition.x / cameraRect.width, (tilePosition.y + m_SelectedRegion.height) / cameraRect.height) };
	m_HighlightVertices[6] = m_HighlightVertices[5];
	m_HighlightVertices[7] = m_HighlightVertices[0];

	m_pHighlightVertexBuffer->UpdateData(m_HighlightVertices);
}

void GridPanel::RenderGrid()
{
	Renderer* pRenderer = Renderer::GetRenderer();
	pRenderer->SetViewport(m_Viewport);
	pRenderer->SetPrimitiveTopology(PrimitiveTopology::LINES);

	pRenderer->BindInputLayout(m_pInputLayout);
	pRenderer->BindVertexBuffer(m_pGridVertexBuffer);
	pRenderer->BindShaderState(m_pVertexShader);
	pRenderer->BindShaderState(m_pGridPixelShader);

	pRenderer->Draw(m_GridCurrentVerticesCount);
	pRenderer->SetPrimitiveTopology(PrimitiveTopology::TRIANGLES);
}

void GridPanel::RenderChunks()
{
	Renderer* pRenderer = Renderer::GetRenderer();
	pRenderer->SetViewport(m_Viewport);
	pRenderer->SetPrimitiveTopology(PrimitiveTopology::LINES);

	pRenderer->BindInputLayout(m_pInputLayout);
	pRenderer->BindVertexBuffer(m_pChunkVertexBuffer);
	pRenderer->BindShaderState(m_pVertexShader);
	pRenderer->BindShaderState(m_pChunkPixelShader);

	pRenderer->Draw(m_ChunkCurrentVerticesCount);
	pRenderer->SetPrimitiveTopology(PrimitiveTopology::TRIANGLES);
}

void GridPanel::RenderHighlight()
{
	if (!m_IsRegionSelected)
		return;

	Renderer* pRenderer = Renderer::GetRenderer();
	pRenderer->SetViewport(m_Viewport);
	pRenderer->SetPrimitiveTopology(PrimitiveTopology::LINES);

	pRenderer->BindInputLayout(m_pInputLayout);
	pRenderer->BindVertexBuffer(m_pHighlightVertexBuffer);
	pRenderer->BindShaderState(m_pVertexShader);
	pRenderer->BindShaderState(m_pHighlightPixelShader);

	pRenderer->Draw(m_HighlightVerticesCount);
	pRenderer->SetPrimitiveTopology(PrimitiveTopology::TRIANGLES);
}

void GridPanel::SetTileClickEvent(const TileClickEvent& event)
{
	m_TileClickEvent = event;
}

bool GridPanel::IsPointInGrid(const Point2f& point) const
{
	return m_Rect.IsInside(point);
}