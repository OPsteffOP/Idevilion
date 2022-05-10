// Folder: Panels

#include "LevelEditorPCH.h"
#include "EditorPanel.h"

#include "TilesheetPanel.h"
#include "InputManager.h"
#include "Renderer.h"
#include "PixelBuffer.h"
#include "PrefabFactory.h"
#include "GraphicsFactory.h"
#include "GameObject.h"
#include "RenderComponent.h"
#include "Camera.h"

EditorPanel::EditorPanel(const Rect4f& rect, TilesheetPanel* pTilesheetPanel)
	: GridPanel(rect)
	, m_pTilesheetPanel(pTilesheetPanel)
	, m_SelectedLayer(0)
	, m_ShouldDrawGrid(true)
{
	SetTileClickEvent([this]()
		{
			if (!m_pTilesheetPanel->IsRegionSelected())
				return;

			const Rect4f& destinationRegion = GetSelectedRegion();
			const Rect4f& selectedRegion = m_pTilesheetPanel->GetSelectedRegion();

			PixelBuffer* pTilesheetImage = m_pTilesheetPanel->GetTilesheetImage();

			Rect4f srcRect = selectedRegion;
			srcRect.x /= pTilesheetImage->GetWidth();
			srcRect.y = pTilesheetImage->GetHeight() - srcRect.y - m_pTilesheetPanel->GetTileSize().y;
			srcRect.y /= pTilesheetImage->GetHeight();
			srcRect.width /= pTilesheetImage->GetWidth();
			srcRect.height /= pTilesheetImage->GetHeight();

			Scene& scene = GetScene(m_SelectedLayer);
			scene.RemoveLastGameObjectAtLocation(Point2f(destinationRegion.x, destinationRegion.y));
			scene.AddGameObject(PrefabFactory::CreateTextureRenderObject(pTilesheetImage, destinationRegion, srcRect));
		});
}

void EditorPanel::Update()
{
	UpdateInputGridToggle();
	UpdateInputTileDeletion();

	GridPanel::Update();

	for (uint i = 0; i < LAYER_COUNT; ++i)
	{
		m_Scenes[i].Update();
	}
}

void EditorPanel::UpdateInputGridToggle()
{
	KeyboardInputDevice* pKeyboardDevice = static_cast<KeyboardInputDevice*>(InputManager::GetInstance()->GetDevice(DeviceType::KEYBOARD));
	if (pKeyboardDevice->IsKeyDown(InputAction::KEYBOARD_LCONTROL) && pKeyboardDevice->IsKeyUp(InputAction::KEYBOARD_G))
		m_ShouldDrawGrid = !m_ShouldDrawGrid;
}

void EditorPanel::UpdateInputTileDeletion()
{
	KeyboardInputDevice* pKeyboardDevice = static_cast<KeyboardInputDevice*>(InputManager::GetInstance()->GetDevice(DeviceType::KEYBOARD));
	if (pKeyboardDevice->IsKeyUp(InputAction::KEYBOARD_DELETE) && m_IsRegionSelected)
	{
		const Point2f selectedPosition = Point2f(GetSelectedRegion().x, GetSelectedRegion().y);
		GetScene(m_SelectedLayer).RemoveLastGameObjectAtLocation(selectedPosition);
	}
}

void EditorPanel::Render()
{
	Renderer* pRenderer = Renderer::GetRenderer();
	pRenderer->SetViewport(m_Viewport);

	for (uint i = 0; i < LAYER_COUNT; ++i)
	{
		m_Scenes[i].Render();
	}

	if (m_ShouldDrawGrid)
	{
		GridPanel::RenderGrid();
		GridPanel::RenderChunks();
		GridPanel::RenderHighlight();
	}
}

void EditorPanel::Save(const std::string& path)
{
	std::ofstream output(path);

	for (uint i = 0; i < LAYER_COUNT; ++i)
	{
		uint layerIndex = i;

		for (const std::pair<Point2i, Chunk>& chunkPair : m_Scenes[i].GetLoadedChunks())
		{
			for (GameObject* pGameObject : chunkPair.second.pGameObjects)
			{
				RenderComponent* pRenderComponent = pGameObject->GetComponent<RenderComponent>();
				const Rect4f& srcRect = pRenderComponent->GetSourceRect();

				output << layerIndex << " " <<
					srcRect.x << "," << srcRect.y << "," << srcRect.width << "," << srcRect.height << " " <<
					pGameObject->GetPosition().x << "," << pGameObject->GetPosition().y << "," << pRenderComponent->GetDestWidth() << "," << pRenderComponent->GetDestHeight() << " " <<
					pRenderComponent->GetTexture()->GetImagePath() << std::endl;
			}
		}
	}
}

void EditorPanel::Open(const std::string& path)
{
	std::ifstream input(path);

	std::string line;
	while (std::getline(input, line))
	{
		const size_t firstSpacePos = line.find(' ');
		const size_t secondSpacePos = line.find(' ', firstSpacePos + 1);
		const size_t thirdSpacePos = line.find(' ', secondSpacePos + 1);

		const std::string firstPart = line.substr(0, firstSpacePos);
		const std::string secondPart = line.substr(firstSpacePos + 1, secondSpacePos);
		const std::string thirdPart = line.substr(secondSpacePos + 1, thirdSpacePos);
		const std::string fourthPart = line.substr(thirdSpacePos + 1);

		const uint layerIndex = (uint)std::stoul(firstPart);

		size_t firstCommaPos = secondPart.find(',');
		size_t secondCommaPos = secondPart.find(',', firstCommaPos + 1);
		size_t thirdCommaPos = secondPart.find(',', secondCommaPos + 1);

		const float srcPosX = std::stof(secondPart.substr(0, firstCommaPos));
		const float srcPosY = std::stof(secondPart.substr(firstCommaPos + 1, secondCommaPos));
		const float srcWidth = std::stof(secondPart.substr(secondCommaPos + 1, thirdCommaPos));
		const float srcHeight = std::stof(secondPart.substr(thirdCommaPos + 1));

		firstCommaPos = thirdPart.find(',');
		secondCommaPos = thirdPart.find(',', firstCommaPos + 1);
		thirdCommaPos = thirdPart.find(',', secondCommaPos + 1);

		const float dstPosX = std::stof(thirdPart.substr(0, firstCommaPos));
		const float dstPosY = std::stof(thirdPart.substr(firstCommaPos + 1, secondCommaPos));
		const float dstWidth = std::stof(thirdPart.substr(secondCommaPos + 1, thirdCommaPos));
		const float dstHeight = std::stof(thirdPart.substr(thirdCommaPos + 1));

		const std::string& texturePath = fourthPart;

		m_Scenes[layerIndex].AddGameObject(PrefabFactory::CreateTextureRenderObject(texturePath, Rect4f(dstPosX, dstPosY, dstWidth, dstHeight), Rect4f(srcPosX, srcPosY, srcWidth, srcHeight)));
	}
}

void EditorPanel::Export(const std::string& path)
{
	// Get all unique chunk indices in all scenes that have at least 1 gameobject
	std::unordered_set<Point2i, Point2i::Hasher> chunkIndices;
	for (uint i = 0; i < LAYER_COUNT; ++i)
	{
		for (const std::pair<Point2i, Chunk>& chunkPair : m_Scenes[i].GetLoadedChunks())
		{
			if (!chunkPair.second.pGameObjects.empty())
				chunkIndices.insert(chunkPair.first);
		}
	}

	uint currentFileLocation = 0;
	std::unordered_map<Point2i, uint, Point2i::Hasher> chunkFileLocations; // file locations without counting header

	// Generating static textures and writing scene data to temp file
	const std::string fileName = std::filesystem::path(path).filename().string();
	const std::string parentFolder = std::filesystem::path(path).parent_path().string();
	const std::string tempFilePath = std::filesystem::path(parentFolder).append("_TEMP_" + fileName).string();
	{
		Renderer* pRenderer = Renderer::GetRenderer();

		PixelBuffer* pColorPixelBuffer = GraphicsFactory::CreatePixelBuffer(Scene::CHUNK_SIZE.x, Scene::CHUNK_SIZE.y, PixelFormat::R8G8B8A8_UNORM,
			BufferUsage::DEFAULT, ShaderAccessFlags::NO_ACCESS, CPUAccessFlags::NO_ACCESS, BindFlags::RENDER_TARGET);
		RenderTarget* pRenderTarget = GraphicsFactory::CreateRenderTarget(pColorPixelBuffer, nullptr);

		Viewport viewport;
		viewport.topLeftX = 0.f;
		viewport.topLeftY = 0.f;
		viewport.width = (float)Scene::CHUNK_SIZE.x;
		viewport.height = (float)Scene::CHUNK_SIZE.y;
		viewport.minDepth = 0.f;
		viewport.maxDepth = 1.f;
		pRenderer->SetViewport(viewport);

		ByteStream tempOutput(new ByteStreamBinaryFileImpl(tempFilePath));
		for (const Point2i& chunkIndex : chunkIndices)
		{
			CameraManager::GetInstance()->GetActiveCamera()->SetCenterPosition((chunkIndex.x * Scene::CHUNK_SIZE.x) + (Scene::CHUNK_SIZE.x / 2.f),
				(chunkIndex.y * Scene::CHUNK_SIZE.y) + (Scene::CHUNK_SIZE.y / 2.f));
			CameraManager::GetInstance()->Update();

			chunkFileLocations.emplace(chunkIndex, currentFileLocation);

			pRenderer->ClearRenderTarget(pRenderTarget, Color{ 0.f, 0.f, 0.0f, 0.f });
			pRenderer->BindRenderTarget(pRenderTarget);
			currentFileLocation += ExportStaticChunkToTexture(parentFolder, "bg", chunkIndex, 0, BACKGROUND_LAYER_COUNT - 1, pColorPixelBuffer, tempOutput);

			pRenderer->ClearRenderTarget(pRenderTarget, Color{ 0.f, 0.f, 0.0f, 0.f });
			pRenderer->BindRenderTarget(pRenderTarget);
			currentFileLocation += ExportStaticChunkToTexture(parentFolder, "fg", chunkIndex, BACKGROUND_LAYER_COUNT, BACKGROUND_LAYER_COUNT + FOREGROUND_LAYER_COUNT - 1, pColorPixelBuffer, tempOutput);
		}

		SAFE_DELETE(pRenderTarget);
	}

	// Writing headers and scene data to final file
	{
		ByteStream output(new ByteStreamBinaryFileImpl(path, true));

		// Writing header
		const uint chunkCount = (uint)chunkIndices.size();
		output.WriteUInt32(chunkCount);

		const uint headerByteSize = (uint)sizeof(uint) + (((uint)sizeof(Point2i) + (uint)sizeof(uint)) * chunkCount);
		for (const Point2i& chunkIndex : chunkIndices)
		{
			const uint chunkFileLocation = headerByteSize + chunkFileLocations[chunkIndex];

			output.WritePoint2i(chunkIndex);
			output.WriteUInt32(chunkFileLocation);
		}
	}

	// Copying static textures and scene data from temp file
	{
		std::ifstream tempFileInput(tempFilePath, std::ios::binary | std::ios::in);
		std::ofstream output(path, std::ios::binary | std::ios::out | std::ios::app | std::ios::ate);
		output << tempFileInput.rdbuf();
	}

	std::filesystem::remove(tempFilePath);
}

uint EditorPanel::ExportStaticChunkToTexture(const std::string& folderPath, const std::string& filePrefix, const Point2i& chunkIndex, uint sceneStart, uint sceneEnd, PixelBuffer* pColorPixelBuffer, ByteStream& output)
{
	for (uint i = sceneStart; i <= sceneEnd; ++i)
	{
		const auto& loadedChunks = m_Scenes[i].GetLoadedChunks();
		if (loadedChunks.find(chunkIndex) == loadedChunks.end())
			continue;

		for (GameObject* pGameObject : loadedChunks.at(chunkIndex).pGameObjects)
		{
			pGameObject->Render();
		}
	}

	const std::string staticTexturePath = std::filesystem::path(folderPath).append(filePrefix + std::to_string(chunkIndex.x) + "," + std::to_string(chunkIndex.y) + ".png").string();
	pColorPixelBuffer->SaveToTexture(staticTexturePath);

	const std::string staticTextureRelativePath = std::filesystem::relative(staticTexturePath, Paths::Data::DATA_DIR).string();
	const uint staticTextureRelativePathSize = (uint)staticTextureRelativePath.size();
	output.WriteUInt32(staticTextureRelativePathSize);
	output.WriteString(staticTextureRelativePath);

	return (uint)sizeof(staticTextureRelativePathSize) + staticTextureRelativePathSize;
}