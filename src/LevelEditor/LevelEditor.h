#pragma once
#include "Game.h"
#include "CameraManager.h"

class Window;
class Swapchain;
class Camera;

class TilesheetPanel;
class EditorPanel;

class UIColorPanel;
class UILabel;

class LevelEditor : public Game
{
public:
	virtual ~LevelEditor() override;

	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Render() override;

private:
	void Save();
	void Open();
	void Export();
	void SelectTileSheet();

private:
	constexpr static const uint MAIN_WINDOW_WIDTH = 1536;
	constexpr static const uint MAIN_WINDOW_HEIGHT = 750;

	constexpr static const uint MENU_WIDTH = MAIN_WINDOW_WIDTH;
	constexpr static const uint MENU_HEIGHT = 30;
	constexpr static const uint MENU_POS_X = 0;
	constexpr static const uint MENU_POS_Y = MAIN_WINDOW_HEIGHT - MENU_HEIGHT;

	constexpr static const uint EDITOR_WIDTH = 1280;
	constexpr static const uint EDITOR_HEIGHT = 720;
	constexpr static const uint EDITOR_POS_X = 0;
	constexpr static const uint EDITOR_POS_Y = 0;

	constexpr static const uint TILESHEET_WIDTH = 256;
	constexpr static const uint TILESHEET_HEIGHT = 256;
	constexpr static const uint TILESHEET_POS_X = EDITOR_POS_X + EDITOR_WIDTH;
	constexpr static const uint TILESHEET_POS_Y = 0;

	constexpr static const uint LAYER_TITLE_WIDTH = 256;
	constexpr static const uint LAYER_TITLE_HEIGHT = 16;
	constexpr static const uint LAYER_TITLE_POS_X = EDITOR_POS_X + EDITOR_WIDTH;
	constexpr static const uint LAYER_TITLE_POS_Y = MAIN_WINDOW_HEIGHT - LAYER_TITLE_HEIGHT - MENU_HEIGHT;

	constexpr static const uint LAYER_SELECTED_WIDTH = 256;
	constexpr static const uint LAYER_SELECTED_HEIGHT = 48;
	constexpr static const uint LAYER_SELECTED_POS_X = EDITOR_POS_X + EDITOR_WIDTH;
	constexpr static const uint LAYER_SELECTED_POS_Y = MAIN_WINDOW_HEIGHT - LAYER_TITLE_HEIGHT - LAYER_SELECTED_HEIGHT - MENU_HEIGHT;

	constexpr static const uint LAYER_SELECTION_WIDTH = 256;
	constexpr static const uint LAYER_SELECTION_HEIGHT = 400;
	constexpr static const uint LAYER_SELECTION_POS_X = EDITOR_POS_X + EDITOR_WIDTH;
	constexpr static const uint LAYER_SELECTION_POS_Y = MAIN_WINDOW_HEIGHT - LAYER_SELECTION_HEIGHT - LAYER_SELECTED_HEIGHT - LAYER_TITLE_HEIGHT - MENU_HEIGHT;

	constexpr static const Color BUTTON_HOVER_COLOR = Color{ 0.f, 0.75f, 0.f, 1.f };
	constexpr static const Color BUTTON_NON_HOVER_COLOR = Color{ 1.f, 1.f, 1.f, 1.f };

	Window* m_pMainWindow;
	Swapchain* m_pMainSwapchain;

	TilesheetPanel* m_pTilesheetPanel;
	EditorPanel* m_pEditorPanel;

	UILabel* m_pLayerSelectedLabel;
	UIColorPanel* m_pLayerScrollPanel;

	Camera* m_pEditorCamera;
	Camera* m_pTilesheetCamera;
	Camera* m_pExportCamera;
};