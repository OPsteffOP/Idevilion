// Folder: Graphics/Text

#include "EnginePCH.h"
#include "Font.h"

#include "GraphicsFactory.h"
#include "WindowManager.h"
#include "CameraManager.h"
#include "Camera.h"

#define STBI_ONLY_PNG
#define STBI_ONLY_JPEG
#include "ImageLoading.h"

uint Font::m_IndexCount = 0;
Buffer* Font::m_pIndexBuffer = nullptr;
Buffer* Font::m_pVertexBuffer = nullptr;
ShaderState* Font::m_pVertexShader = nullptr;
ShaderState* Font::m_pDefaultPixelShader = nullptr;
InputLayout* Font::m_pInputLayout = nullptr;
Sampler* Font::m_pDefaultSampler = nullptr;
Buffer* Font::m_pCharacterConstantBuffer = nullptr;
Buffer* Font::m_pTextColorConstantBuffer = nullptr;

struct CharacterData
{
	float srcTopLeftX;
	float srcTopLeftY;
	float srcWidth;
	float srcHeight;

	float dstPositionX;
	float dstPositionY;
	float dstWidth;
	float dstHeight;
};

struct TextColorData
{
	Color color;
};

Font::Font(const std::string& bpfFilePath)
{
	ByteStream input(new ByteStreamBinaryFileImpl(bpfFilePath));

	const uint charactersCount = input.ReadUInt32();
	for (uint i = 0; i < charactersCount; ++i)
	{
		const char character = input.ReadChar();
		m_CharacterData[character] = input.ReadRect4f();
	}

	const int textureDataPNGLength = input.ReadInt32();

	unsigned char* pTextureDataPNG = new unsigned char[textureDataPNGLength];
	input.ReadData(pTextureDataPNG, (uint)textureDataPNGLength, Endian::BIG_ENDIAN);

	int bitmapWidth;
	int bitmapHeight;
	int channelCount;
	unsigned char* pTextureData = stbi_load_from_memory(pTextureDataPNG, textureDataPNGLength, &bitmapWidth, &bitmapHeight, &channelCount, 1);

	SAFE_DELETE_ARRAY(pTextureDataPNG);

	m_pPixelBuffer = GraphicsFactory::CreatePixelBuffer(pTextureData, bitmapWidth, bitmapHeight, PixelFormat::R8_UNORM, BufferUsage::IMMUTABLE,
		ShaderAccessFlags::READ, CPUAccessFlags::NO_ACCESS, BindFlags::NONE);

	stbi_image_free(pTextureData);

	if (m_pIndexBuffer == nullptr)
		CreateBuffers();
	m_pPixelShader = m_pDefaultPixelShader;
}

Font::~Font()
{
	if (m_pPixelShader != m_pDefaultPixelShader)
		SAFE_DELETE(m_pPixelShader);

	SAFE_DELETE(m_pPixelBuffer);
}

void Font::CreateBuffers()
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
	m_pVertexShader = GraphicsFactory::CreateShaderState(ShaderType::VERTEX_SHADER, Paths::Data::DATA_DIR + Paths::Data::SHADERS_DIR + "DefaultTextVS.hlsl");
	ALLOW_NEXT_ALLOCATION_LEAK();
	m_pDefaultPixelShader = GraphicsFactory::CreateShaderState(ShaderType::PIXEL_SHADER, Paths::Data::DATA_DIR + Paths::Data::SHADERS_DIR + "DefaultTextPS.hlsl");

	std::vector<InputElement> inputElements;
	inputElements.reserve(2);
	inputElements.push_back(InputElement{ "POSITION", 0, PixelFormat::R32G32_FLOAT, InputElementType::VERTEX_DATA });
	inputElements.push_back(InputElement{ "TEXCOORD", 0, PixelFormat::R32G32_FLOAT, InputElementType::VERTEX_DATA });
	ALLOW_NEXT_ALLOCATION_LEAK();
	m_pInputLayout = GraphicsFactory::CreateInputLayout(m_pVertexShader, inputElements);

	ALLOW_NEXT_ALLOCATION_LEAK();
	m_pDefaultSampler = GraphicsFactory::CreateSampler(SamplerFilter::POINT, RepeatMode::BORDER_COLOR, Color{ 0.f, 0.f, 0.f, 1.f });

	ALLOW_NEXT_ALLOCATION_LEAK();
	m_pCharacterConstantBuffer = GraphicsFactory::CreateBuffer((uint)sizeof(CharacterData), 1, BufferUsage::DYNAMIC,
		ShaderAccessFlags::NO_ACCESS, CPUAccessFlags::WRITE, BindFlags::CONSTANT_BUFFER);

	ALLOW_NEXT_ALLOCATION_LEAK();
	m_pTextColorConstantBuffer = GraphicsFactory::CreateBuffer((uint)sizeof(TextColorData), 1, BufferUsage::DYNAMIC,
		ShaderAccessFlags::NO_ACCESS, CPUAccessFlags::WRITE, BindFlags::CONSTANT_BUFFER);
}

float Font::GetTextWidth(const std::string& text, uint fontSize)
{
	float width = 0.f;
	for (char character : text)
	{
		if (character == ' ')
		{
			width += fontSize / 2.f;
			continue;
		}

		const Rect4f& characterRect = m_CharacterData[character];
		const float characterAspectRatio = (float)characterRect.width / characterRect.height;

		width += fontSize * characterAspectRatio;
	}

	return width;
}

void Font::DrawText(const std::string& text, uint fontSize, const Color& textColor, const Point2f& destPosition, bool isScreenSpace)
{
	const uint windowWidth = WindowManager::GetInstance()->GetCurrentWindow()->GetWidth();
	const uint windowHeight = WindowManager::GetInstance()->GetCurrentWindow()->GetHeight();

	Viewport viewport;
	viewport.topLeftX = destPosition.x;
	viewport.topLeftY = windowHeight - destPosition.y - fontSize;
	viewport.width = (float)windowWidth;
	viewport.height = (float)fontSize;
	viewport.minDepth = 0.f;
	viewport.maxDepth = 1.f;

	// TODO: zooming

	if (!isScreenSpace)
	{
		const Rect4f& cameraRect = CameraManager::GetInstance()->GetActiveCamera()->GetWorldRectangleInView();
		viewport.topLeftX -= cameraRect.x;
		viewport.topLeftY += cameraRect.y;
	}

	DrawText(text, fontSize, textColor, viewport);
}

void Font::DrawText(const std::string& text, uint fontSize, const Color& textColor, const Rect4f& destRect, bool isScreenSpace)
{
	const uint windowHeight = WindowManager::GetInstance()->GetCurrentWindow()->GetHeight();
	const Camera* pCamera = CameraManager::GetInstance()->GetActiveCamera();

	Viewport viewport;
	viewport.topLeftX = destRect.x;
	viewport.topLeftY = windowHeight - destRect.y - destRect.height;
	viewport.width = destRect.width;
	viewport.height = destRect.height;
	viewport.minDepth = 0.f;
	viewport.maxDepth = 1.f;

	// TODO: zooming

	if (!isScreenSpace)
	{
		const Rect4f& cameraRect = pCamera->GetWorldRectangleInView();
		viewport.width += cameraRect.x;
		viewport.height += cameraRect.y;
		viewport.topLeftX -= cameraRect.x;
		viewport.topLeftY += cameraRect.y + (destRect.height - viewport.height);
	}

	DrawText(text, fontSize, textColor, viewport);
}

void Font::DrawText(const std::string& text, uint fontSize, const Color& textColor, const Viewport& viewport)
{
	// Setting render state - stays the same for all characters
	Renderer* pRenderer = Renderer::GetRenderer();

	pRenderer->SetPrimitiveTopology(PrimitiveTopology::TRIANGLES);
	pRenderer->SetViewport(viewport);

	pRenderer->BindIndexBuffer(m_pIndexBuffer);
	pRenderer->BindVertexBuffer(m_pVertexBuffer);
	pRenderer->BindShaderState(m_pVertexShader);
	pRenderer->BindShaderState(m_pPixelShader);
	pRenderer->BindInputLayout(m_pInputLayout);
	pRenderer->BindReadBufferPS(m_pPixelBuffer, 0);
	pRenderer->BindConstantBufferVS(m_pCharacterConstantBuffer, 0);
	pRenderer->BindConstantBufferPS(m_pTextColorConstantBuffer, 0);

	TextColorData textColorData;
	textColorData.color = textColor;
	m_pTextColorConstantBuffer->UpdateData(&textColorData);

	float currentX = 0.f;
	for (char character : text)
	{
		if (character == ' ')
		{
			currentX += (fontSize / 2.f) / viewport.width;
			continue;
		}

		// Updating character cbuffer
		const Rect4f& characterRect = m_CharacterData[character];
		const float characterAspectRatio = (float)characterRect.width / characterRect.height;

		CharacterData data;
		data.srcTopLeftX = characterRect.x / m_pPixelBuffer->GetWidth();
		data.srcTopLeftY = characterRect.y / m_pPixelBuffer->GetHeight();
		data.srcWidth = characterRect.width / m_pPixelBuffer->GetWidth();
		data.srcHeight = characterRect.height / m_pPixelBuffer->GetHeight();
		data.dstPositionX = currentX;
		data.dstPositionY = 0.f;
		data.dstWidth = (fontSize * characterAspectRatio) / viewport.width;
		data.dstHeight = fontSize / viewport.height;

		m_pCharacterConstantBuffer->UpdateData(&data);

		pRenderer->DrawIndexed(m_IndexCount);

		currentX += data.dstWidth;
	}
}

// TODO: zooming
// TODO: fit it in a component for the world space, so it doesn't always draw when it's not in view
	// screen space should be drawn always though