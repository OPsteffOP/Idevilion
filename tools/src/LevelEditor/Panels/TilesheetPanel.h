// Folder: Panels

#pragma once
#include "GridPanel.h"

class GameObject;
class PixelBuffer;

class TilesheetPanel : public GridPanel
{
public:
	TilesheetPanel(const Rect4f& rect);
	virtual ~TilesheetPanel() override;

	virtual void Update() override;
	void Render();

	void SetTilesheet(const std::string& tilesheetPath);
	PixelBuffer* GetTilesheetImage() const;

private:
	GameObject* m_pTilesheetObject;
};