// Folder: Graphics/UI

#pragma once
#include "Viewport.h"

class RMUISystem;

class RenderTarget;
class Buffer;
class PixelBuffer;
class ShaderState;
class InputLayout;
class Sampler;

class Font;

class UIElement
{
	friend class RMUISystem;

public:
	UIElement(const Rect4f& rect);
	virtual ~UIElement();

	virtual void Update() = 0;
	virtual void Render() = 0;

	virtual void AddChildElement(UIElement* pElement);
	virtual void RemoveChildElement(UIElement* pElement, bool shouldDelete = true);

	const Rect4f& GetRect() const { return m_Rect; }
	const Rect4f& GetHierarchyRect() const { return m_HierarchyRect; }
	void SetRect(const Rect4f& rect);
	void SetVisible(bool isVisible);


protected:
	bool m_IsVisible;
	Rect4f m_Rect;
	Rect4f m_HierarchyRect;
	Rect4f m_NormalizedHierarchyRect;

	UIElement* m_pParent;
	std::vector<UIElement*> m_pChildren;
	RMUISystem* m_pSystem;

	Viewport m_Viewport;

private:
	void InternalUpdate();
	void InternalRender();

	void UpdateRect();

private:
	struct PositionData
	{
		float positionX;
		float positionY;
		float width;
		float height;
	};
	Buffer* m_pPositionDataBuffer;
};

class UIScrollPanel : public UIElement
{
public:
	UIScrollPanel(const Rect4f& rect, float scrollSpeed = 0.01f);

	virtual void Update() override;
	virtual void Render() override {}

	virtual void AddChildElement(UIElement* pElement) override;
	virtual void RemoveChildElement(UIElement* pElement, bool shouldDelete = true) override;

protected:
	void UpdateScrolling();

protected:
	float m_ScrollSpeed;
	float m_ScrollPositionLimit;
	float m_ScrollPositionLimitOffset;
	float m_ScrollPosition;
};

class UIColorPanel : public UIScrollPanel
{
public:
	UIColorPanel(const Rect4f& rect, const Color& color);
	virtual ~UIColorPanel() override;

	virtual void Render() override;

	void SetColor(const Color& color);

protected:
	Color m_CurrentColor;

private:
	void CreateBuffers();

	void UpdateColor();

private:
	static uint m_IndexCount;
	static Buffer* m_pIndexBuffer;
	static Buffer* m_pVertexBuffer;
	static ShaderState* m_pVertexShader;
	static ShaderState* m_pPixelShader;
	static InputLayout* m_pInputLayout;

	struct ColorData
	{
		Color color;
	};
	Buffer* m_pColorDataBuffer;
};

class UIButton : public UIColorPanel
{
public:
	UIButton(const Rect4f& rect, const Color& color, const std::function<void(UIButton* pButton)>& clickEventHandler);

	virtual void Update() override;

	void SetEnterEventHandler(const std::function<void(UIButton* pButton)>& enterEventHandler);
	void SetLeaveEventHandler(const std::function<void(UIButton* pButton)>& leaveEventHandler);

private:
	std::function<void(UIButton* pButton)> m_ClickEventHandler;

	std::function<void(UIButton* pButton)> m_EnterEventHandler;
	std::function<void(UIButton* pButton)> m_LeaveEventHandler;
	bool m_IsEntered;
};

class UILabel : public UIElement
{
public:
	UILabel(const Rect4f& rect, const std::string& text, Font* pFont, uint fontSize, const Color& textColor, bool shouldCenterText = false);

	virtual void Update() override {}
	virtual void Render() override;

	void SetText(const std::string& text);

private:
	bool m_ShouldCenterText;
	std::string m_Text;
	Font* m_pFont;
	uint m_FontSize;
	Color m_TextColor;
};