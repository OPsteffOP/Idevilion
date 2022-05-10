// Folder: Panels

#pragma once
#include "GridPanel.h"
#include "Scene.h"
#include "ByteStream.h"

class TilesheetPanel;
class PixelBuffer;

class EditorPanel : public GridPanel
{
public:
	EditorPanel(const Rect4f& rect, TilesheetPanel* pTilesheetPanel);

	virtual void Update() override;
	void Render();

	Scene& GetScene(uint layer) { return m_Scenes[layer]; }
	void SetSelectedLayer(uint layer) { m_SelectedLayer = layer; }

	void Save(const std::string& path);
	void Open(const std::string& path);
	void Export(const std::string& path);

	constexpr static const uint BACKGROUND_LAYER_COUNT = 5;
	constexpr static const uint FOREGROUND_LAYER_COUNT = 5;
	constexpr static const uint LAYER_COUNT = BACKGROUND_LAYER_COUNT + FOREGROUND_LAYER_COUNT;

private:
	void UpdateInputGridToggle();
	void UpdateInputTileDeletion();

	uint ExportStaticChunkToTexture(const std::string& folderPath, const std::string& filePrefix, const Point2i& chunkIndex, uint sceneStart, uint sceneEnd, PixelBuffer* pColorPixelBuffer, ByteStream& output);

private:
	TilesheetPanel* m_pTilesheetPanel;

	uint m_SelectedLayer;

	Scene m_Scenes[LAYER_COUNT];
	bool m_ShouldDrawGrid;
};