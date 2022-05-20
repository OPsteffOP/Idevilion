#include "LevelEditorPCH.h"
#include "LevelEditor.h"

#include "WindowManager.h"
#include "InputManager.h"
#include "GraphicsFactory.h"
#include "Camera.h"

#include "EditorPanel.h"
#include "TilesheetPanel.h"

#include "FontBaking.h"
#include "FontCache.h"

#include "RetainedModeUISystem.h"
#include "RetainedModeUIElements.h"

#include "SaveOpenDialog.h"

LevelEditor::~LevelEditor()
{
	SAFE_DELETE(m_pTilesheetPanel);
	SAFE_DELETE(m_pEditorPanel);

	SAFE_DELETE(m_pEditorCamera);
	SAFE_DELETE(m_pTilesheetCamera);
	SAFE_DELETE(m_pExportCamera);
}

void LevelEditor::Initialize()
{
	Renderer* pRenderer = GraphicsFactory::CreateRenderer(RenderAPI::PlatformDefault);
	RendererFlags initFlags = RendererFlags::NONE;
#ifdef BP_CONFIGURATION_DEBUG
	initFlags |= RendererFlags::ENABLE_DEBUGGING;
#endif
	pRenderer->Initialize(initFlags);
	Renderer::SetRenderer(pRenderer);

	// Don't launch level editor, just bake
	if (CommandLine::IsFlagSet("--bake") || CommandLine::IsFlagSet("--bake-and-run"))
	{
		Baking::BakeFontSDFBitmap("Arial"); // TODO: don't hardcode which font to bake
		if (CommandLine::IsFlagSet("--bake"))
			return;
	}

	m_pMainWindow = WindowManager::GetInstance()->CreateWindow(WindowIdentifier::MAIN_WINDOW, "Level Editor", MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT);
	WindowManager::GetInstance()->SetCurrentWindow(WindowIdentifier::MAIN_WINDOW);

	m_pMainSwapchain = GraphicsFactory::CreateSwapchain(MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT, m_pMainWindow->GetWindowHandle());
	m_pMainWindow->SetSwapchain(m_pMainSwapchain);

	m_pEditorCamera = new Camera(Point2f(EDITOR_WIDTH / 2.f, EDITOR_HEIGHT / 2.f), Point2f((float)EDITOR_WIDTH, (float)EDITOR_HEIGHT), 0.1f, 5.f);
	m_pTilesheetCamera = new Camera(Point2f(TILESHEET_WIDTH / 2.f, TILESHEET_HEIGHT / 2.f), Point2f((float)TILESHEET_WIDTH, (float)TILESHEET_HEIGHT), 0.1f, 5.f);
	m_pExportCamera = new Camera(Point2f(Scene::CHUNK_SIZE.x / 2.f, Scene::CHUNK_SIZE.y / 2.f), Point2f((float)Scene::CHUNK_SIZE.x, (float)Scene::CHUNK_SIZE.y), 0.1f, 5.f);

	m_pTilesheetPanel = new TilesheetPanel(Rect4f((float)TILESHEET_POS_X, (float)TILESHEET_POS_Y, (float)TILESHEET_WIDTH, (float)TILESHEET_HEIGHT));
	m_pEditorPanel = new EditorPanel(Rect4f((float)EDITOR_POS_Y, (float)EDITOR_POS_Y, (float)EDITOR_WIDTH, (float)EDITOR_HEIGHT), m_pTilesheetPanel);

	UIColorPanel* pMenuPanel = new UIColorPanel(Rect4f(MENU_POS_X, MENU_POS_Y, MENU_WIDTH, MENU_HEIGHT), Color{ 0.75f, 0.75f, 0.75f, 1.f });
	RMUISystem::GetSystemForCurrentWindow()->AddElement(pMenuPanel);

	UIButton* pSaveButton = new UIButton(Rect4f(4.f / MENU_WIDTH, 0.5f - ((25 / 2.f) / MENU_HEIGHT), 90.f / MENU_WIDTH, 25.f / MENU_HEIGHT),
		Color{ 1.f, 1.f, 1.f, 1.f }, [this](UIButton*) { Save(); });
	pSaveButton->SetEnterEventHandler([this](UIButton* pButton) { pButton->SetColor(BUTTON_HOVER_COLOR); });
	pSaveButton->SetLeaveEventHandler([this](UIButton* pButton) { pButton->SetColor(BUTTON_NON_HOVER_COLOR); });
	pMenuPanel->AddChildElement(pSaveButton);
	UILabel* pSaveLabel = new UILabel(Rect4f(0.f, 0.f, 1.f, 1.f), "Save", FontCache::GetInstance()->GetFont("Arial"), 16, Color{ 0.f, 0.f, 0.f, 1.f }, true);
	pSaveButton->AddChildElement(pSaveLabel);

	UIButton* pOpenButton = new UIButton(Rect4f(pSaveButton->GetRect().x + pSaveButton->GetRect().width + (4.f / MENU_WIDTH), 0.5f - ((25 / 2.f) / MENU_HEIGHT), 90.f / MENU_WIDTH, 25.f / MENU_HEIGHT),
		Color{ 1.f, 1.f, 1.f, 1.f }, [this](UIButton*) { Open(); });
	pOpenButton->SetEnterEventHandler([this](UIButton* pButton) { pButton->SetColor(BUTTON_HOVER_COLOR); });
	pOpenButton->SetLeaveEventHandler([this](UIButton* pButton) { pButton->SetColor(BUTTON_NON_HOVER_COLOR); });
	pMenuPanel->AddChildElement(pOpenButton);
	UILabel* pOpenLabel = new UILabel(Rect4f(0.f, 0.f, 1.f, 1.f), "Open", FontCache::GetInstance()->GetFont("Arial"), 16, Color{ 0.f, 0.f, 0.f, 1.f }, true);
	pOpenButton->AddChildElement(pOpenLabel);

	UIButton* pExportButton = new UIButton(Rect4f(pOpenButton->GetRect().x + pOpenButton->GetRect().width + (4.f / MENU_WIDTH), 0.5f - ((25 / 2.f) / MENU_HEIGHT), 90.f / MENU_WIDTH, 25.f / MENU_HEIGHT),
		Color{ 1.f, 1.f, 1.f, 1.f }, [this](UIButton*) { Export(); });
	pExportButton->SetEnterEventHandler([this](UIButton* pButton) { pButton->SetColor(BUTTON_HOVER_COLOR); });
	pExportButton->SetLeaveEventHandler([this](UIButton* pButton) { pButton->SetColor(BUTTON_NON_HOVER_COLOR); });
	pMenuPanel->AddChildElement(pExportButton);
	UILabel* pExportLabel = new UILabel(Rect4f(0.f, 0.f, 1.f, 1.f), "Export", FontCache::GetInstance()->GetFont("Arial"), 16, Color{ 0.f, 0.f, 0.f, 1.f }, true);
	pExportButton->AddChildElement(pExportLabel);

	UIButton* pTilesheetButton = new UIButton(Rect4f(pExportButton->GetRect().x + pExportButton->GetRect().width + (32.f / MENU_WIDTH), 0.5f - ((25 / 2.f) / MENU_HEIGHT), 200.f / MENU_WIDTH, 25.f / MENU_HEIGHT),
		Color{ 1.f, 1.f, 1.f, 1.f }, [this](UIButton*) { SelectTileSheet(); });
	pTilesheetButton->SetEnterEventHandler([this](UIButton* pButton) { pButton->SetColor(BUTTON_HOVER_COLOR); });
	pTilesheetButton->SetLeaveEventHandler([this](UIButton* pButton) { pButton->SetColor(BUTTON_NON_HOVER_COLOR); });
	pMenuPanel->AddChildElement(pTilesheetButton);
	UILabel* pTilesheetLabel = new UILabel(Rect4f(0.f, 0.f, 1.f, 1.f), "Select TileSheet", FontCache::GetInstance()->GetFont("Arial"), 16, Color{ 0.f, 0.f, 0.f, 1.f }, true);
	pTilesheetButton->AddChildElement(pTilesheetLabel);

	UILabel* pLayerTitleLabel = new UILabel(Rect4f(LAYER_TITLE_POS_X, LAYER_TITLE_POS_Y, LAYER_TITLE_WIDTH, LAYER_TITLE_HEIGHT), "SELECTED LAYER:", FontCache::GetInstance()->GetFont("Arial"), 12, Color{ 1.f, 1.f, 1.f, 1.f }, true);
	m_pLayerSelectedLabel = new UILabel(Rect4f(LAYER_SELECTED_POS_X, LAYER_SELECTED_POS_Y, LAYER_SELECTED_WIDTH, LAYER_SELECTED_HEIGHT), "0", FontCache::GetInstance()->GetFont("Arial"), 32, Color{ 1.f, 1.f, 1.f, 1.f }, true);
	RMUISystem::GetSystemForCurrentWindow()->AddElement(pLayerTitleLabel);
	RMUISystem::GetSystemForCurrentWindow()->AddElement(m_pLayerSelectedLabel);

	m_pLayerScrollPanel = new UIColorPanel(Rect4f(LAYER_SELECTION_POS_X, LAYER_SELECTION_POS_Y, LAYER_SELECTION_WIDTH, LAYER_SELECTION_HEIGHT), Color{ 0.5f, 0.5f, 0.5f, 1.f });
	RMUISystem::GetSystemForCurrentWindow()->AddElement(m_pLayerScrollPanel);

	constexpr const uint layerCount = EditorPanel::LAYER_COUNT;
	const float layerSelectButtonHeight = 32.f / LAYER_SELECTION_HEIGHT;
	const float layerSelectButtonPaddingX = 8.f / LAYER_SELECTION_WIDTH;
	const float layerSelectButtonPaddingY = 8.f / LAYER_SELECTION_HEIGHT;
	for (uint i = 0; i < layerCount; ++i)
	{
		UIButton* pButton = new UIButton(Rect4f(layerSelectButtonPaddingX, 1.f - ((layerSelectButtonHeight + layerSelectButtonPaddingY) * (i + 1)), 1.f - (layerSelectButtonPaddingX * 2.f), layerSelectButtonHeight),
			Color{ 1.f, 1.f, 1.f, 1.f }, [this, i](UIButton*) { m_pEditorPanel->SetSelectedLayer(i); m_pLayerSelectedLabel->SetText(std::to_string(i)); });
		pButton->SetEnterEventHandler([this](UIButton* pButton) { pButton->SetColor(BUTTON_HOVER_COLOR); });
		pButton->SetLeaveEventHandler([this](UIButton* pButton) { pButton->SetColor(BUTTON_NON_HOVER_COLOR); });

		m_pLayerScrollPanel->AddChildElement(pButton);

		UILabel* pLabel = new UILabel(Rect4f(0.f, 0.f, 1.f, 1.f), "Layer " + std::to_string(i), FontCache::GetInstance()->GetFont("Arial"), 16, Color{ 0.f, 0.f, 0.f, 1.f }, true);
		pButton->AddChildElement(pLabel);
	}
}

void LevelEditor::Update()
{
	InputManager::GetInstance()->Update();

	m_pMainWindow->Update();

	WindowManager::GetInstance()->SetCurrentWindow(WindowIdentifier::MAIN_WINDOW);
	Renderer* pRenderer = Renderer::GetRenderer();
	pRenderer->ClearRenderTarget(m_pMainSwapchain->GetDefaultRenderTarget(), Color{ 0.f, 0.f, 0.0f, 1.f });
	pRenderer->BindRenderTarget(m_pMainSwapchain->GetDefaultRenderTarget());

	CameraManager::GetInstance()->SetActiveCamera(m_pEditorCamera);
	m_pEditorPanel->Update();
	CameraManager::GetInstance()->Update();
	CameraManager::GetInstance()->SetActiveCamera(m_pTilesheetCamera);
	m_pTilesheetPanel->Update();
	CameraManager::GetInstance()->Update();

	RMUISystem::GetSystemForCurrentWindow()->Update();
}

void LevelEditor::Render()
{
	CameraManager::GetInstance()->SetActiveCamera(m_pEditorCamera);
	m_pEditorPanel->Render();
	CameraManager::GetInstance()->Render();
	CameraManager::GetInstance()->SetActiveCamera(m_pTilesheetCamera);
	m_pTilesheetPanel->Render();
	CameraManager::GetInstance()->Render();

	RMUISystem::GetSystemForCurrentWindow()->Render();

	m_pMainSwapchain->Present();
}

void LevelEditor::Save()
{
	std::vector<SaveOpenDialog::DialogFileType> types;
	types.push_back({ std::string("Level editor file (*") + Paths::Data::Extensions::WORLD_FILE_NON_OPTIMIZED + ")", std::string("*") + Paths::Data::Extensions::WORLD_FILE_NON_OPTIMIZED});
	SaveOpenDialog* pDialog = SaveOpenDialog::Create(SaveOpenDialogFlags::NO_READ_ONLY | SaveOpenDialogFlags::PROMPT_ON_OVERWRITE, types, Paths::Data::Extensions::WORLD_FILE_NON_OPTIMIZED, Paths::Data::DATA_DEV_DIR + Paths::Data::WORLDS_DIR);

	pDialog->Show();
	const std::string savePath = pDialog->GetSelectedPath();
	SAFE_DELETE(pDialog);

	if (!savePath.empty())
		m_pEditorPanel->Save(savePath);
}

void LevelEditor::Open()
{
	std::vector<SaveOpenDialog::DialogFileType> types;
	types.push_back({ std::string("Level editor file (*") + Paths::Data::Extensions::WORLD_FILE_NON_OPTIMIZED + ")", std::string("*") + Paths::Data::Extensions::WORLD_FILE_NON_OPTIMIZED });
	SaveOpenDialog* pDialog = SaveOpenDialog::Create(SaveOpenDialogFlags::FORCE_PATH_EXISTANCE | SaveOpenDialogFlags::FORCE_FILE_EXISTANCE, types, Paths::Data::Extensions::WORLD_FILE_NON_OPTIMIZED, Paths::Data::DATA_DEV_DIR + Paths::Data::WORLDS_DIR);

	pDialog->Show();
	const std::string openPath = pDialog->GetSelectedPath();
	SAFE_DELETE(pDialog);

	if (!openPath.empty())
		m_pEditorPanel->Open(openPath);
}

void LevelEditor::Export()
{
	std::vector<SaveOpenDialog::DialogFileType> types;
	types.push_back({ std::string("World file (*") + Paths::Data::Extensions::WORLD_FILE + ")", std::string("*") + Paths::Data::Extensions::WORLD_FILE });
	SaveOpenDialog* pDialog = SaveOpenDialog::Create(SaveOpenDialogFlags::NO_READ_ONLY | SaveOpenDialogFlags::PROMPT_ON_OVERWRITE, types, Paths::Data::Extensions::WORLD_FILE, Paths::Data::DATA_DIR + Paths::Data::WORLDS_DIR);

	pDialog->Show();
	const std::string savePath = pDialog->GetSelectedPath();
	SAFE_DELETE(pDialog);

	if (!savePath.empty())
	{
		Camera* pPreviousActiveCamera = CameraManager::GetInstance()->GetActiveCamera();
		CameraManager::GetInstance()->SetActiveCamera(m_pExportCamera);
		m_pEditorPanel->Export(savePath);
		CameraManager::GetInstance()->SetActiveCamera(pPreviousActiveCamera);
	}
}

void LevelEditor::SelectTileSheet()
{
	std::vector<SaveOpenDialog::DialogFileType> types;
	types.push_back({ "Image file (*.png)", "*.png" });
	SaveOpenDialog* pDialog = SaveOpenDialog::Create(SaveOpenDialogFlags::FORCE_PATH_EXISTANCE | SaveOpenDialogFlags::FORCE_FILE_EXISTANCE, types, ".png", Paths::Data::DATA_DEV_DIR + Paths::Data::TEXTURES_DIR);

	pDialog->Show();
	const std::string openPath = pDialog->GetSelectedPath();
	SAFE_DELETE(pDialog);

	if (!openPath.empty())
		m_pTilesheetPanel->SetTilesheet(openPath);
}