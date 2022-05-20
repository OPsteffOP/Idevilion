// Folder: Panels

#pragma once
#include "Viewport.h"
#include "CameraManager.h"

class Buffer;
class ShaderState;
class InputLayout;
class ExclusiveInputDevice;

class GridPanel
{
public:
	typedef std::function<void()> TileClickEvent;

public:
	GridPanel(const Rect4f& rect);
	virtual ~GridPanel();

	virtual void Update();
	virtual void RenderGrid();
	virtual void RenderChunks();
	virtual void RenderHighlight();

	void TEMP_SetTileSize(float x, float y)
	{
		if (std::fabsf(x - m_TileSize.x) <= FLT_EPSILON && std::fabsf(y - m_TileSize.x) <= FLT_EPSILON)
			return;

		m_TileSize.x = x;
		m_TileSize.y = y;
		m_IsDirty = true;

		m_IsRegionSelected = false;
	}

	void SetTileClickEvent(const TileClickEvent& event);

	const Rect4f& GetRect() const { return m_Rect; }
	const Point2f& GetTileSize() const { return m_TileSize; }
	const Rect4f& GetSelectedRegion() const { return m_SelectedRegion; }
	bool IsRegionSelected() const { return m_IsRegionSelected; }
	bool IsPointInGrid(const Point2f& point) const;

protected:
	struct Vertex
	{
		Point2f position;
	};

	Rect4f m_Rect;

	Point2f m_TileSize;
	Point2f m_TileOffset;
	bool m_IsDirty;

	Viewport m_Viewport;

	constexpr static uint m_GridMaxVerticesCount = 1024;
	uint m_GridCurrentVerticesCount;
	Vertex m_GridVertices[m_GridMaxVerticesCount];
	Buffer* m_pGridVertexBuffer;

	constexpr static uint m_ChunkMaxVerticesCount = 1024;
	uint m_ChunkCurrentVerticesCount;
	Vertex m_ChunkVertices[m_ChunkMaxVerticesCount];
	Buffer* m_pChunkVertexBuffer;

	constexpr static uint m_HighlightVerticesCount = 8;
	Vertex m_HighlightVertices[m_HighlightVerticesCount];
	Buffer* m_pHighlightVertexBuffer;

	ShaderState* m_pVertexShader;
	InputLayout* m_pInputLayout;

	ShaderState* m_pGridPixelShader;
	ShaderState* m_pChunkPixelShader;
	ShaderState* m_pHighlightPixelShader;

	ExclusiveInputDevice* m_pExclusiveMouseDevice;
	bool m_IsWorldMoving;
	bool m_MouseMovedDuringExclusiveMode;

	bool m_IsRegionSelected;
	bool m_DidRegionSelectionChange;
	Rect4f m_SelectedRegion;

	TileClickEvent m_TileClickEvent;

private:
	void UpdateInput();
	void UpdateInputGridMovement();
	void UpdateInputWorldZoom();
	void UpdateInputGridZoom();
	void UpdateInputTileSelection();

	void UpdateGrid();
	void UpdateChunks();
	void UpdateHighlight();
};