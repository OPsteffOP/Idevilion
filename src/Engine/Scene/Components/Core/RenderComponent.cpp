// Folder: Scene/Components/Core

#include "EnginePCH.h"
#include "RenderComponent.h"

#include "GraphicsFactory.h"
#include "Buffer.h"
#include "PixelBuffer.h"
#include "ShaderState.h"
#include "InputLayout.h"
#include "Sampler.h"
#include "Renderer.h"
#include "GameObject.h"
#include "ImageCache.h"

uint RenderComponent::m_IndexCount = 0;
Buffer* RenderComponent::m_pIndexBuffer = nullptr;
Buffer* RenderComponent::m_pVertexBuffer = nullptr;
ShaderState* RenderComponent::m_pDefaultVertexShader = nullptr;
ShaderState* RenderComponent::m_pDefaultPixelShader = nullptr;
InputLayout* RenderComponent::m_pDefaultInputLayout = nullptr;
Sampler* RenderComponent::m_pDefaultSampler = nullptr;

struct TextureBufferElement
{
	float destPositionX;
	float destPositionY;
	float destWidth;
	float destHeight;

	float srcPositionX;
	float srcPositionY;
	float srcWidth;
	float srcHeight;
};

RenderComponent::RenderComponent(const std::string& texturePath, float destWidth, float destHeight)
	: RenderComponent(texturePath, destWidth, destHeight, Rect4f(0.f, 0.f, 1.f, 1.f))
{}

RenderComponent::RenderComponent(const std::string& texturePath, float destWidth, float destHeight, const Rect4f& srcRect)
	: RenderComponent(ImageCache::GetInstance()->GetImage(texturePath), destWidth, destHeight, srcRect)
{}

RenderComponent::RenderComponent(PixelBuffer* pPixelBuffer, float destWidth, float destHeight, const Rect4f& srcRect)
	: m_pVertexShader(nullptr)
	, m_pPixelShader(nullptr)
	, m_pInputLayout(nullptr)
	, m_pSampler(nullptr)
	, m_pTexture(pPixelBuffer)
	, m_pConstantBuffer(nullptr)
	, m_SrcRect(srcRect)
	, m_DestWidth(destWidth)
	, m_DestHeight(destHeight)
	, m_ShouldForceUpdate(false)
{
	if (m_pIndexBuffer == nullptr)
		CreateBuffers();

	m_pVertexShader = m_pDefaultVertexShader;
	m_pPixelShader = m_pDefaultPixelShader;
	m_pInputLayout = m_pDefaultInputLayout;
	m_pSampler = m_pDefaultSampler;

	if (m_DestWidth == USE_TEXTURE_WIDTH)
		m_DestWidth = (float)m_pTexture->GetWidth();
	if (m_DestHeight == USE_TEXTURE_HEIGHT)
		m_DestHeight = (float)m_pTexture->GetHeight();
}

RenderComponent::~RenderComponent()
{
	if (m_pVertexShader != m_pDefaultVertexShader)
		SAFE_DELETE(m_pVertexShader);

	if (m_pPixelShader != m_pDefaultPixelShader)
		SAFE_DELETE(m_pPixelShader);

	if (m_pInputLayout != m_pDefaultInputLayout)
		SAFE_DELETE(m_pInputLayout);

	if (m_pSampler != m_pDefaultSampler)
		SAFE_DELETE(m_pSampler);

	SAFE_DELETE(m_pConstantBuffer);
}

void RenderComponent::CreateBuffers()
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
	m_pDefaultVertexShader = GraphicsFactory::CreateShaderState(ShaderType::VERTEX_SHADER, Paths::Data::DATA_DIR + Paths::Data::SHADERS_DIR + "DefaultVS.hlsl");
	ALLOW_NEXT_ALLOCATION_LEAK();
	m_pDefaultPixelShader = GraphicsFactory::CreateShaderState(ShaderType::PIXEL_SHADER, Paths::Data::DATA_DIR + Paths::Data::SHADERS_DIR + "DefaultPS.hlsl");

	std::vector<InputElement> inputElements;
	inputElements.reserve(2);
	inputElements.push_back(InputElement{ "POSITION", 0, PixelFormat::R32G32_FLOAT, InputElementType::VERTEX_DATA });
	inputElements.push_back(InputElement{ "TEXCOORD", 0, PixelFormat::R32G32_FLOAT, InputElementType::VERTEX_DATA });
	ALLOW_NEXT_ALLOCATION_LEAK();
	m_pDefaultInputLayout = GraphicsFactory::CreateInputLayout(m_pDefaultVertexShader, inputElements);

	ALLOW_NEXT_ALLOCATION_LEAK();
	m_pDefaultSampler = GraphicsFactory::CreateSampler(SamplerFilter::LINEAR, RepeatMode::BORDER_COLOR, Color{ 0.f, 0.f, 0.f, 0.f });
}

void RenderComponent::Initialize()
{
	const Point2f& position = m_pGameObject->GetPosition();

	TextureBufferElement data;
	data.destPositionX = position.x;
	data.destPositionY = position.y;
	data.destWidth = m_DestWidth;
	data.destHeight = m_DestHeight;
	data.srcPositionX = m_SrcRect.x;
	data.srcPositionY = m_SrcRect.y;
	data.srcWidth = m_SrcRect.width;
	data.srcHeight = m_SrcRect.height;

	m_pConstantBuffer = GraphicsFactory::CreateBuffer(&data, (uint)sizeof(TextureBufferElement), 1, BufferUsage::DYNAMIC,
		ShaderAccessFlags::NO_ACCESS, CPUAccessFlags::WRITE, BindFlags::CONSTANT_BUFFER);
}

void RenderComponent::Update()
{
	if (m_pGameObject->IsPositionDirty() || m_ShouldForceUpdate)
	{
		const Point2f& position = m_pGameObject->GetPosition();

		TextureBufferElement data;
		data.destPositionX = position.x;
		data.destPositionY = position.y;
		data.destWidth = m_DestWidth;
		data.destHeight = m_DestHeight;
		data.srcPositionX = m_SrcRect.x;
		data.srcPositionY = m_SrcRect.y;
		data.srcWidth = m_SrcRect.width;
		data.srcHeight = m_SrcRect.height;

		m_pConstantBuffer->UpdateData(&data);

		m_ShouldForceUpdate = false;
	}
}

void RenderComponent::Render()
{
	Renderer* pRenderer = Renderer::GetRenderer();

	pRenderer->BindInputLayout(m_pInputLayout);
	pRenderer->BindIndexBuffer(m_pIndexBuffer);
	pRenderer->BindVertexBuffer(m_pVertexBuffer);
	pRenderer->BindShaderState(m_pVertexShader);
	pRenderer->BindShaderState(m_pPixelShader);
	pRenderer->BindSamplerPS(m_pSampler, 0);
	pRenderer->BindReadBufferPS(m_pTexture, 0);
	pRenderer->BindConstantBufferVS(m_pConstantBuffer, 0);

	pRenderer->DrawIndexed(m_IndexCount);
}