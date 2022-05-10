// Folder: Panels

#include "LevelEditorPCH.h"
#include "TilesheetPanel.h"

#include "Renderer.h"
#include "PrefabFactory.h"
#include "GameObject.h"
#include "RenderComponent.h"

TilesheetPanel::TilesheetPanel(const Rect4f& rect)
	: GridPanel(rect)
	, m_pTilesheetObject(nullptr)
{}

TilesheetPanel::~TilesheetPanel()
{
	SAFE_DELETE(m_pTilesheetObject);
}

void TilesheetPanel::Update()
{
	GridPanel::Update();

	if (m_pTilesheetObject != nullptr)
		m_pTilesheetObject->Update();
}

void TilesheetPanel::Render()
{
	Renderer* pRenderer = Renderer::GetRenderer();
	pRenderer->SetViewport(m_Viewport);

	if (m_pTilesheetObject != nullptr)
		m_pTilesheetObject->Render();

	GridPanel::RenderGrid();
	GridPanel::RenderChunks();
	GridPanel::RenderHighlight();
}

void TilesheetPanel::SetTilesheet(const std::string& tilesheetPath)
{
	SAFE_DELETE(m_pTilesheetObject);
	m_pTilesheetObject = PrefabFactory::CreateTextureRenderObject(tilesheetPath, Rect4f(0.f, 0.f, USE_TEXTURE_WIDTH, USE_TEXTURE_HEIGHT));
}

PixelBuffer* TilesheetPanel::GetTilesheetImage() const
{
	if (m_pTilesheetObject == nullptr)
		return nullptr;

	return m_pTilesheetObject->GetComponent<RenderComponent>()->GetTexture();
}