// Folder: Graphics/UI

#include "EnginePCH.h"
#include "RetainedModeUIElements.h"

#include "RetainedModeUISystem.h"
#include "GraphicsFactory.h"
#include "InputManager.h"
#include "Font.h"

uint UIColorPanel::m_IndexCount = 0;
Buffer* UIColorPanel::m_pIndexBuffer = nullptr;
Buffer* UIColorPanel::m_pVertexBuffer = nullptr;
ShaderState* UIColorPanel::m_pVertexShader = nullptr;
ShaderState* UIColorPanel::m_pPixelShader = nullptr;
InputLayout* UIColorPanel::m_pInputLayout = nullptr;

UIElement::UIElement(const Rect4f& rect)
	: m_IsVisible(true)
	, m_Rect(rect)
	, m_pSystem(nullptr)
{
	m_pPositionDataBuffer = GraphicsFactory::CreateBuffer((uint)sizeof(PositionData), 1, BufferUsage::DYNAMIC, ShaderAccessFlags::NO_ACCESS, CPUAccessFlags::WRITE, BindFlags::CONSTANT_BUFFER);

	UpdateRect();
}

UIElement::~UIElement()
{
	for (UIElement* pChild : m_pChildren)
	{
		SAFE_DELETE(pChild);
	}

	SAFE_DELETE(m_pPositionDataBuffer);
}

void UIElement::InternalUpdate()
{
	if (m_IsVisible)
	{
		Update();

		for (UIElement* pChildElement : m_pChildren)
		{
			pChildElement->InternalUpdate();
		}
	}
}

void UIElement::InternalRender()
{
	if (m_IsVisible)
	{
		Renderer* pRenderer = Renderer::GetRenderer();
		pRenderer->BindConstantBufferVS(m_pPositionDataBuffer, 0);
		pRenderer->SetViewport(m_Viewport);

		Render();

		for (UIElement* pChildElement : m_pChildren)
		{
			pChildElement->InternalRender();
		}
	}
}

void UIElement::AddChildElement(UIElement* pElement)
{
	m_pChildren.push_back(pElement);
	pElement->m_pParent = this;
	pElement->m_pSystem = m_pSystem;

	UpdateRect();
	for (UIElement* pChild : m_pChildren)
	{
		pChild->UpdateRect();
	}

	if (m_pSystem != nullptr)
	{
		m_pSystem->MarkDirty();

		for (UIElement* pChild : pElement->m_pChildren)
		{
			pChild->m_pSystem = m_pSystem;
		}
	}
}

void UIElement::RemoveChildElement(UIElement* pElement, bool shouldDelete)
{
	m_pChildren.erase(std::remove(m_pChildren.begin(), m_pChildren.end(), pElement));

	if (m_pSystem != nullptr)
		m_pSystem->MarkDirty();

	pElement->m_pParent = nullptr;
	pElement->m_pSystem = nullptr;

	UpdateRect();
	for (UIElement* pChild : m_pChildren)
	{
		pChild->UpdateRect();
	}

	for (UIElement* pChild : pElement->m_pChildren)
	{
		pChild->m_pSystem = m_pSystem;
	}

	if (shouldDelete)
		delete pElement;
}

void UIElement::SetRect(const Rect4f& rect)
{
	m_Rect = rect;
	UpdateRect();

	for (UIElement* pChild : m_pChildren)
	{
		pChild->UpdateRect();
	}

	if (m_IsVisible)
	{
		if (m_pSystem != nullptr)
			m_pSystem->MarkDirty();
	}
}

void UIElement::SetVisible(bool isVisible)
{
	if (m_IsVisible != isVisible)
	{
		m_IsVisible = isVisible;

		if (m_pSystem != nullptr)
			m_pSystem->MarkDirty();
	}
}

void UIElement::UpdateRect()
{
	Window* pWindow = WindowManager::GetInstance()->GetCurrentWindow();

	m_HierarchyRect = m_Rect;
	if (m_pParent != nullptr)
	{
		m_HierarchyRect.x = m_pParent->m_HierarchyRect.x + (m_Rect.x * m_pParent->m_HierarchyRect.width);
		m_HierarchyRect.y = m_pParent->m_HierarchyRect.y + (m_Rect.y * m_pParent->m_HierarchyRect.height);
		m_HierarchyRect.width = m_Rect.width * m_pParent->m_HierarchyRect.width;
		m_HierarchyRect.height = m_Rect.height * m_pParent->m_HierarchyRect.height;
	}

	m_NormalizedHierarchyRect = Rect4f(0.f, 0.f, 1.f, 1.f);
	if (m_pParent != nullptr)
	{
		UIElement* pRootElement = m_pParent;
		while (pRootElement->m_pParent != nullptr)
		{
			pRootElement = pRootElement->m_pParent;
		}

		m_NormalizedHierarchyRect.x = (m_HierarchyRect.x - pRootElement->m_Rect.x) / pRootElement->m_Rect.width;
		m_NormalizedHierarchyRect.y = (m_HierarchyRect.y - pRootElement->m_Rect.y) / pRootElement->m_Rect.height;
		m_NormalizedHierarchyRect.width = m_HierarchyRect.width / pRootElement->m_Rect.width;
		m_NormalizedHierarchyRect.height = m_HierarchyRect.height / pRootElement->m_Rect.height;
	}

	if (m_pParent == nullptr)
	{
		m_Viewport.topLeftX = m_Rect.x;
		m_Viewport.topLeftY = pWindow->GetHeight() - (m_Rect.y + m_Rect.height);
		m_Viewport.width = m_Rect.width;
		m_Viewport.height = m_Rect.height;
		m_Viewport.minDepth = 0.f;
		m_Viewport.maxDepth = 1.f;
	}
	else
	{
		m_Viewport = m_pParent->m_Viewport;
	}

	PositionData data;
	if (m_pParent == nullptr)
	{
		data.positionX = 0.f;
		data.positionY = 0.f;
		data.width = 1.f;
		data.height = 1.f;
	}
	else
	{
		data.positionX = m_NormalizedHierarchyRect.x;
		data.positionY = m_NormalizedHierarchyRect.y;
		data.width = m_NormalizedHierarchyRect.width;
		data.height = m_NormalizedHierarchyRect.height;
	}
	m_pPositionDataBuffer->UpdateData(&data);
}

UIScrollPanel::UIScrollPanel(const Rect4f& rect, float scrollSpeed)
	: UIElement(rect)
	, m_ScrollSpeed(scrollSpeed)
	, m_ScrollPositionLimit(0.f)
	, m_ScrollPositionLimitOffset(FLT_MAX)
	, m_ScrollPosition(0.f)
{}

void UIScrollPanel::Update()
{
	UpdateScrolling();
}

void UIScrollPanel::AddChildElement(UIElement* pElement)
{
	const float childPositionY = pElement->GetRect().y;
	m_ScrollPositionLimitOffset = std::min(m_ScrollPositionLimitOffset, 1.f - childPositionY - pElement->GetRect().height);
	m_ScrollPositionLimit = std::max(m_ScrollPositionLimit, -childPositionY + m_ScrollPositionLimitOffset); // TODO: the "+ m_ScrollPositionLimitOffset" here is very be AddChildElement order dependent, top elements should be inserted before bottom elements

	Rect4f rect = pElement->GetRect();
	rect.y += m_ScrollPosition;
	pElement->SetRect(rect);

	UIElement::AddChildElement(pElement);
}

void UIScrollPanel::RemoveChildElement(UIElement* pElement, bool shouldDelete)
{
	Rect4f rect = pElement->GetRect();
	rect.y -= m_ScrollPosition;
	pElement->SetRect(rect);

	UIElement::RemoveChildElement(pElement, shouldDelete);
}

void UIScrollPanel::UpdateScrolling()
{
	Window* pWindow = WindowManager::GetInstance()->GetCurrentWindow();
	Point2f clientMousePosition = pWindow->GetLocalMousePosition();
	clientMousePosition.y = pWindow->GetHeight() - clientMousePosition.y;

	if (m_HierarchyRect.IsInside(clientMousePosition))
	{
		MouseInputDevice* pMouseDevice = static_cast<MouseInputDevice*>(InputManager::GetInstance()->GetDevice(DeviceType::MOUSE));
		const float scrollDistance = pMouseDevice->GetState(InputAction::MOUSE_SCROLL);
		if (std::fabsf(scrollDistance) >= FLT_EPSILON)
		{
			if (m_pChildren.empty() || std::fabsf(m_ScrollPositionLimit) <= FLT_EPSILON)
				return;

			const float previousScrollPosition = m_ScrollPosition;
			m_ScrollPosition -= scrollDistance * m_ScrollSpeed;
			m_ScrollPosition = std::clamp(m_ScrollPosition, 0.f, m_ScrollPositionLimit);
			const float scrollDelta = m_ScrollPosition - previousScrollPosition;

			for (UIElement* pChild : m_pChildren)
			{
				Rect4f rect = pChild->GetRect();
				rect.y += scrollDelta;
				pChild->SetRect(rect);

				// TODO: scroll bar
			}
		}
	}
}

UIColorPanel::UIColorPanel(const Rect4f& rect, const Color& color)
	: UIScrollPanel(rect)
	, m_CurrentColor(color)
{
	m_pColorDataBuffer = GraphicsFactory::CreateBuffer((uint)sizeof(ColorData), 1, BufferUsage::DYNAMIC, ShaderAccessFlags::NO_ACCESS, CPUAccessFlags::WRITE, BindFlags::CONSTANT_BUFFER);

	if (m_pIndexBuffer == nullptr)
		CreateBuffers();

	UpdateColor();
}

UIColorPanel::~UIColorPanel()
{
	SAFE_DELETE(m_pColorDataBuffer);
}

void UIColorPanel::CreateBuffers()
{
	constexpr uint indexCount = 6;
	m_IndexCount = indexCount;

	int16_t indices[indexCount]
	{
		0, 2, 1,
		1, 2, 3
	};
	ALLOW_NEXT_ALLOCATION_LEAK();
	m_pIndexBuffer = GraphicsFactory::CreateBuffer(indices, (uint)sizeof(int16_t), indexCount, BufferUsage::IMMUTABLE,
		ShaderAccessFlags::NO_ACCESS, CPUAccessFlags::NO_ACCESS, BindFlags::INDEX_BUFFER);

	struct Vertex
	{
		Point2f position;
	};

	constexpr uint vertexCount = 4;
	Vertex vertices[vertexCount]
	{
		Vertex{ Point2f(-1.f, -1.f) },
		Vertex{ Point2f(1.f, -1.f) },
		Vertex{ Point2f(-1.f,  1.f) },
		Vertex{ Point2f(1.f,  1.f) }
	};
	ALLOW_NEXT_ALLOCATION_LEAK();
	m_pVertexBuffer = GraphicsFactory::CreateBuffer(vertices, (uint)sizeof(Vertex), vertexCount, BufferUsage::IMMUTABLE,
		ShaderAccessFlags::NO_ACCESS, CPUAccessFlags::NO_ACCESS, BindFlags::VERTEX_BUFFER);

	ALLOW_NEXT_ALLOCATION_LEAK();
	m_pVertexShader = GraphicsFactory::CreateShaderState(ShaderType::VERTEX_SHADER, Paths::Data::DATA_DIR + Paths::Data::SHADERS_DIR + "UI\\UIColorPanelVS.hlsl");
	ALLOW_NEXT_ALLOCATION_LEAK();
	m_pPixelShader = GraphicsFactory::CreateShaderState(ShaderType::PIXEL_SHADER, Paths::Data::DATA_DIR + Paths::Data::SHADERS_DIR + "UI\\UIColorPanelPS.hlsl");

	std::vector<InputElement> inputElements;
	inputElements.reserve(1);
	inputElements.push_back(InputElement{ "POSITION", 0, PixelFormat::R32G32_FLOAT, InputElementType::VERTEX_DATA });
	ALLOW_NEXT_ALLOCATION_LEAK();
	m_pInputLayout = GraphicsFactory::CreateInputLayout(m_pVertexShader, inputElements);
}

void UIColorPanel::Render()
{
	Renderer* pRenderer = Renderer::GetRenderer();
	pRenderer->SetPrimitiveTopology(PrimitiveTopology::TRIANGLES);

	pRenderer->BindInputLayout(m_pInputLayout);
	pRenderer->BindIndexBuffer(m_pIndexBuffer);
	pRenderer->BindVertexBuffer(m_pVertexBuffer);
	pRenderer->BindShaderState(m_pVertexShader);
	pRenderer->BindShaderState(m_pPixelShader);
	pRenderer->BindConstantBufferPS(m_pColorDataBuffer, 0);

	pRenderer->DrawIndexed(m_IndexCount);
}

void UIColorPanel::SetColor(const Color& color)
{
	m_CurrentColor = color;
	m_pSystem->MarkDirty();

	UpdateColor();
}

void UIColorPanel::UpdateColor()
{
	ColorData data;
	data.color = m_CurrentColor;
	m_pColorDataBuffer->UpdateData(&data);
}

UIButton::UIButton(const Rect4f& rect, const Color& color, const std::function<void(UIButton* pButton)>& clickEventHandler)
	: UIColorPanel(rect, color)
	, m_ClickEventHandler(clickEventHandler)
	, m_EnterEventHandler(nullptr)
	, m_LeaveEventHandler(nullptr)
	, m_IsEntered(false)
{}

void UIButton::Update()
{
	Window* pWindow = WindowManager::GetInstance()->GetCurrentWindow();
	Point2f clientMousePosition = pWindow->GetLocalMousePosition();
	clientMousePosition.y = pWindow->GetHeight() - clientMousePosition.y;

	if (m_HierarchyRect.IsInside(clientMousePosition))
	{
		if (!m_IsEntered)
		{
			if (m_EnterEventHandler != nullptr)
				m_EnterEventHandler(this);

			m_IsEntered = true;
		}

		MouseInputDevice* pMouseDevice = static_cast<MouseInputDevice*>(InputManager::GetInstance()->GetDevice(DeviceType::MOUSE));
		if (!pMouseDevice->GetPreviousState(InputAction::MOUSE_LEFT_BUTTON) && pMouseDevice->GetState(InputAction::MOUSE_LEFT_BUTTON))
		{
			if (m_ClickEventHandler != nullptr)
				m_ClickEventHandler(this);
		}
	}
	else if (m_IsEntered)
	{
		if (m_LeaveEventHandler != nullptr)
			m_LeaveEventHandler(this);

		m_IsEntered = false;
	}
}

void UIButton::SetEnterEventHandler(const std::function<void(UIButton* pButton)>& enterEventHandler)
{
	m_EnterEventHandler = enterEventHandler;
}

void UIButton::SetLeaveEventHandler(const std::function<void(UIButton* pButton)>& leaveEventHandler)
{
	m_LeaveEventHandler = leaveEventHandler;
}

UILabel::UILabel(const Rect4f& rect, const std::string& text, Font* pFont, uint fontSize, const Color& textColor, bool shouldCenterText)
	: UIElement(rect)
	, m_ShouldCenterText(shouldCenterText)
	, m_Text(text)
	, m_pFont(pFont)
	, m_FontSize(fontSize)
	, m_TextColor(textColor)
{}

void UILabel::Render()
{
	if (m_ShouldCenterText)
	{
		Rect4f centeredRect = m_HierarchyRect;
		centeredRect.width = m_pFont->GetTextWidth(m_Text, m_FontSize);
		centeredRect.height = (float)m_FontSize;
		centeredRect.x += (m_HierarchyRect.width / 2.f) - (centeredRect.width / 2.f);
		centeredRect.y += (m_HierarchyRect.height / 2.f) - (centeredRect.height / 2.f);
		m_pFont->DrawText(m_Text, m_FontSize, m_TextColor, centeredRect);
	}
	else
	{
		m_pFont->DrawText(m_Text, m_FontSize, m_TextColor, m_HierarchyRect);
	}
}

void UILabel::SetText(const std::string& text)
{
	m_Text = text;

	if (m_pSystem != nullptr)
		m_pSystem->MarkDirty();
}