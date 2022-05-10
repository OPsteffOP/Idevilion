// Folder: Graphics/UI

#include "EnginePCH.h"
#include "RetainedModeUISystem.h"

#include "RetainedModeUIElements.h"
#include "GraphicsFactory.h"

std::unordered_map<WindowIdentifier, RMUISystem*> RMUISystem::m_pSystems;
uint RMUISystem::m_IndexCount = 0;
Buffer* RMUISystem::m_pIndexBuffer = nullptr;
Buffer* RMUISystem::m_pVertexBuffer = nullptr;
ShaderState* RMUISystem::m_pVertexShader = nullptr;
ShaderState* RMUISystem::m_pPixelShader = nullptr;
InputLayout* RMUISystem::m_pInputLayout = nullptr;
Sampler* RMUISystem::m_pSampler = nullptr;

RMUISystem* RMUISystem::GetSystemForCurrentWindow()
{
	const WindowIdentifier windowIdentifier = WindowManager::GetInstance()->GetCurrentWindowIdentifier();
	if (m_pSystems.find(windowIdentifier) == m_pSystems.end())
	{
		ALLOW_NEXT_ALLOCATION_LEAK();
		m_pSystems[windowIdentifier] = new RMUISystem();
	}

	return m_pSystems[windowIdentifier];
}

RMUISystem::RMUISystem()
	: m_IsDirty(true)
{
	Window* pWindow = WindowManager::GetInstance()->GetCurrentWindow();
	const uint width = pWindow->GetWidth();
	const uint height = pWindow->GetHeight();

	m_Viewport.topLeftX = 0.f;
	m_Viewport.topLeftY = 0.f;
	m_Viewport.width = (float)width;
	m_Viewport.height = (float)height;
	m_Viewport.minDepth = 0.f;
	m_Viewport.maxDepth = 1.f;

	m_pRetainedPixelBuffer = GraphicsFactory::CreatePixelBuffer(width, height, PixelFormat::R8G8B8A8_UNORM, BufferUsage::DEFAULT,
		ShaderAccessFlags::READ, CPUAccessFlags::NO_ACCESS, BindFlags::RENDER_TARGET);
	m_pRetainedRenderTarget = GraphicsFactory::CreateRenderTarget(m_pRetainedPixelBuffer, nullptr);

	if (m_pIndexBuffer == nullptr)
		CreateBuffers();
}

void RMUISystem::CreateBuffers()
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
		Point2f texCoord;
	};

	constexpr uint vertexCount = 4;
	Vertex vertices[vertexCount]
	{
		Vertex{ Point2f(-1.f, -1.f), Point2f(0.f, 1.f) },
		Vertex{ Point2f(1.f, -1.f), Point2f(1.f, 1.f) },
		Vertex{ Point2f(-1.f,  1.f), Point2f(0.f, 0.f) },
		Vertex{ Point2f(1.f,  1.f), Point2f(1.f, 0.f) }
	};
	ALLOW_NEXT_ALLOCATION_LEAK();
	m_pVertexBuffer = GraphicsFactory::CreateBuffer(vertices, (uint)sizeof(Vertex), vertexCount, BufferUsage::IMMUTABLE,
		ShaderAccessFlags::NO_ACCESS, CPUAccessFlags::NO_ACCESS, BindFlags::VERTEX_BUFFER);

	ALLOW_NEXT_ALLOCATION_LEAK();
	m_pVertexShader = GraphicsFactory::CreateShaderState(ShaderType::VERTEX_SHADER, Paths::Data::DATA_DIR + Paths::Data::SHADERS_DIR + "UI\\UIBlendingVS.hlsl");
	ALLOW_NEXT_ALLOCATION_LEAK();
	m_pPixelShader = GraphicsFactory::CreateShaderState(ShaderType::PIXEL_SHADER, Paths::Data::DATA_DIR + Paths::Data::SHADERS_DIR + "UI\\UIBlendingPS.hlsl");

	std::vector<InputElement> inputElements;
	inputElements.reserve(2);
	inputElements.push_back(InputElement{ "POSITION", 0, PixelFormat::R32G32_FLOAT, InputElementType::VERTEX_DATA });
	inputElements.push_back(InputElement{ "TEXCOORD", 0, PixelFormat::R32G32_FLOAT, InputElementType::VERTEX_DATA });
	ALLOW_NEXT_ALLOCATION_LEAK();
	m_pInputLayout = GraphicsFactory::CreateInputLayout(m_pVertexShader, inputElements);

	ALLOW_NEXT_ALLOCATION_LEAK();
	m_pSampler = GraphicsFactory::CreateSampler(SamplerFilter::POINT, RepeatMode::BORDER_COLOR, Color{ 0.f, 0.f, 0.f, 1.f });
}

RMUISystem::~RMUISystem()
{
	for (UIElement* pElement : m_pRootElements)
	{
		SAFE_DELETE(pElement);
	}

	SAFE_DELETE(m_pRetainedRenderTarget);
}

void RMUISystem::Update()
{
	for (UIElement* pRootElement : m_pRootElements)
	{
		pRootElement->InternalUpdate();
	}
}

void RMUISystem::Render()
{
	Renderer* pRenderer = Renderer::GetRenderer();

	if (m_IsDirty)
	{
		pRenderer->ClearRenderTarget(m_pRetainedRenderTarget, Color{ 0.f, 0.f, 0.f, 0.f });
		pRenderer->BindRenderTarget(m_pRetainedRenderTarget);

		for (UIElement* pRootElement : m_pRootElements)
		{
			pRootElement->InternalRender();
		}

		m_IsDirty = false;
	}

	Window* pWindow = WindowManager::GetInstance()->GetCurrentWindow();
	Swapchain* pSwapchain = pWindow->GetSwapchain();

	pRenderer->SetViewport(m_Viewport);
	pRenderer->SetPrimitiveTopology(PrimitiveTopology::TRIANGLES);
	pRenderer->BindRenderTarget(pSwapchain->GetDefaultRenderTarget());

	pRenderer->BindInputLayout(m_pInputLayout);
	pRenderer->BindIndexBuffer(m_pIndexBuffer);
	pRenderer->BindVertexBuffer(m_pVertexBuffer);
	pRenderer->BindShaderState(m_pVertexShader);
	pRenderer->BindShaderState(m_pPixelShader);
	pRenderer->BindSamplerPS(m_pSampler, 0);
	pRenderer->BindReadBufferPS(m_pRetainedPixelBuffer, 0);

	pRenderer->DrawIndexed(m_IndexCount);
}

void RMUISystem::AddElement(UIElement* pElement)
{
	m_pRootElements.push_back(pElement);
	pElement->m_pSystem = this;
	MarkDirty();
}

void RMUISystem::RemoveElement(UIElement* pElement, bool shouldDelete)
{
	m_pRootElements.erase(std::remove(m_pRootElements.begin(), m_pRootElements.end(), pElement));
	pElement->m_pSystem = nullptr;
	MarkDirty();

	if (shouldDelete)
		delete pElement;
}