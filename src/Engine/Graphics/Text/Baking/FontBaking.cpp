// Folder: Graphics/Text/Baking

#include "EnginePCH.h"
#include "FontBaking.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "FontLoading.h"

#include "GraphicsFactory.h"
#include "ByteStream.h"

void Baking::BakeFontSDFBitmap(const std::string& fontName)
{
	UNREFERENCED_PARAMETER(fontName);

#ifdef INCLUDE_BAKING_CODE
	ByteStream binaryFontBitmap(new ByteStreamBinaryFileImpl(Paths::Data::DATA_DIR + Paths::Data::FONTS_DIR + fontName + Paths::Data::Extensions::BAKED_SDF_FONT_FILE, true));

	std::string ttfFilePath = Paths::Data::DATA_DEV_DIR + Paths::Data::FONTS_DIR + fontName + ".ttf";
	if (!std::filesystem::exists(ttfFilePath))
	{
		ttfFilePath = Paths::OS::GetFontsFolder() + fontName + ".ttf";
		if (!std::filesystem::exists(ttfFilePath))
		{
			LOG_ERROR("Font (.ttf) to bake not found in either the DATA_DEV_DIR or operating system's font path, font_name=%s", fontName.c_str());
			return;
		}
	}

	// Reading font info
	const unsigned long long fileSize = std::filesystem::file_size(ttfFilePath);
	ByteStream fontFile(new ByteStreamBinaryFileImpl(ttfFilePath));

	unsigned char* pFileData = new unsigned char[fileSize];
	fontFile.ReadData(pFileData, fileSize, Endian::BIG_ENDIAN);

	stbtt_fontinfo fontInfo;
	int initFontStatus = stbtt_InitFont(&fontInfo, pFileData, stbtt_GetFontOffsetForIndex(pFileData, 0));
	if (initFontStatus == 0)
	{
		LOG_ERROR("Failed to initialize ttf font for baking, font_name=%s", fontName.c_str());
		return;
	}

	// Loading characters and creating pixel buffers for them
	constexpr const int startCharacter = '!';
	constexpr const int endCharacter = '~' + 1;
	constexpr const int pixelBuffersCount = endCharacter - startCharacter;
	PixelBuffer* pPixelBuffers[pixelBuffersCount];
	std::fill(std::begin(pPixelBuffers), std::end(pPixelBuffers), nullptr);
	for (int i = startCharacter; i < endCharacter; ++i)
	{
		int glyph = stbtt_FindGlyphIndex(&fontInfo, i);
		if (glyph == 0)
		{
			LOG_ERROR("Font doesn't have character '%c', font_name=%s", (char)i, fontName.c_str());
			continue;
		}

		int width;
		int height;
		int offsetX;
		int offsetY;
		unsigned char* pData = stbtt_GetGlyphSDF(&fontInfo, stbtt_ScaleForPixelHeight(&fontInfo, 512), glyph, 25, 180, 5.f, &width, &height, &offsetX, &offsetY);

		if (pData == nullptr)
		{
			LOG_ERROR("Font doesn't have character '%c', font_name=%s", (char)i, fontName.c_str());
			continue;
		}

		pPixelBuffers[i - startCharacter] = GraphicsFactory::CreatePixelBuffer(pData, width, height, PixelFormat::R8_UINT, BufferUsage::IMMUTABLE,
			ShaderAccessFlags::READ, CPUAccessFlags::NO_ACCESS, BindFlags::NONE);
		//pPixelBuffers[i - startCharacter]->SaveToTexture("C:\\Users\\Administrator\\Perforce\\STEFF-DESKTOP\\P3-Placeholder\\Dev\\data\\test\\" + std::string(1, (char)i) + ".png");

		stbtt_FreeSDF(pData, fontInfo.userdata);
	}

	// Generating bitmap texture with all loaded characters
	constexpr const uint bitmapWidth = 2048;
	constexpr const uint bitmapHeight = 2048;

	Renderer* pRenderer = Renderer::GetRenderer();
	PixelBuffer* pFontBitmapPixelBuffer = GraphicsFactory::CreatePixelBuffer(bitmapWidth, bitmapHeight, PixelFormat::R8_UNORM,
		BufferUsage::DEFAULT, ShaderAccessFlags::NO_ACCESS, CPUAccessFlags::NO_ACCESS, BindFlags::RENDER_TARGET);
	RenderTarget* pRenderTarget = GraphicsFactory::CreateRenderTarget(pFontBitmapPixelBuffer, nullptr);

	pRenderer->SetPrimitiveTopology(PrimitiveTopology::TRIANGLES);
	pRenderer->ClearRenderTarget(pRenderTarget, Color{ 0.f, 0.f, 0.0f, 1.f });
	pRenderer->BindRenderTarget(pRenderTarget);

	constexpr const uint indicesCount = 6;
	int16_t indices[indicesCount]
	{
		0, 2, 1,
		1, 2, 3
	};
	Buffer* pIndexBuffer = GraphicsFactory::CreateBuffer(indices, (uint)sizeof(int16_t), indicesCount, BufferUsage::IMMUTABLE,
		ShaderAccessFlags::NO_ACCESS, CPUAccessFlags::NO_ACCESS, BindFlags::INDEX_BUFFER);

	struct Vertex
	{
		Point2f position;
		Point2f texCoord;
	};
	constexpr const uint verticesCount = 4;
	Vertex vertices[verticesCount]
	{
		Vertex{ Point2f(-1.f, -1.f), Point2f(0.f, 1.f) },
		Vertex{ Point2f(1.f, -1.f), Point2f(1.f, 1.f) },
		Vertex{ Point2f(-1.f,  1.f), Point2f(0.f, 0.f) },
		Vertex{ Point2f(1.f,  1.f), Point2f(1.f, 0.f) }
	};
	Buffer* pVertexBuffer = GraphicsFactory::CreateBuffer(vertices, (uint)sizeof(Vertex), verticesCount, BufferUsage::IMMUTABLE,
		ShaderAccessFlags::NO_ACCESS, CPUAccessFlags::NO_ACCESS, BindFlags::VERTEX_BUFFER);

	ShaderState* pVertexShader = GraphicsFactory::CreateShaderState(ShaderType::VERTEX_SHADER, Paths::Data::DATA_DEV_DIR + Paths::Data::SHADERS_DIR + "BakeFontSDFBitmapVS.hlsl");
	ShaderState* pPixelShader = GraphicsFactory::CreateShaderState(ShaderType::PIXEL_SHADER, Paths::Data::DATA_DEV_DIR + Paths::Data::SHADERS_DIR + "BakeFontSDFBitmapPS.hlsl");

	std::vector<InputElement> inputElements;
	inputElements.push_back(InputElement{ "POSITION", 0, PixelFormat::R32G32_FLOAT, InputElementType::VERTEX_DATA });
	inputElements.push_back(InputElement{ "TEXCOORD", 0, PixelFormat::R32G32_FLOAT, InputElementType::VERTEX_DATA });
	InputLayout* pInputLayout = GraphicsFactory::CreateInputLayout(pVertexShader, inputElements);

	struct Data
	{
		float characterWidth;
		float characterHeight;
		float _reserved[2];
	};
	Buffer* pConstantBuffer = GraphicsFactory::CreateBuffer((uint)sizeof(Data), 1, BufferUsage::DYNAMIC,
		ShaderAccessFlags::NO_ACCESS, CPUAccessFlags::WRITE, BindFlags::CONSTANT_BUFFER);

	const uint maxCharacterWidth = (*std::max_element(std::begin(pPixelBuffers), std::end(pPixelBuffers),
		[](PixelBuffer* pPixelBuffer1, PixelBuffer* pPixelBuffer2) { return pPixelBuffer1->GetWidth() < pPixelBuffer2->GetWidth(); }))->GetWidth();
	const uint maxCharacterHeight = (*std::max_element(std::begin(pPixelBuffers), std::end(pPixelBuffers),
		[](PixelBuffer* pPixelBuffer1, PixelBuffer* pPixelBuffer2) { return pPixelBuffer1->GetHeight() < pPixelBuffer2->GetHeight(); }))->GetHeight();
	const uint maxDimension = std::max(maxCharacterWidth, maxCharacterHeight);

	constexpr const uint characterMaxWidth = 204;
	constexpr const uint characterMaxHeight = 204;
	constexpr const uint charactersPerRow = 10;
	constexpr const float characterPaddingX = 0.8f;
	constexpr const float characterPaddingY = 0.8f;

	const float widthMultiplier = (float)characterMaxWidth / maxDimension;
	const float heightMultiplier = (float)characterMaxHeight / maxDimension;

	const uint charactersCount = (uint)std::count_if(std::begin(pPixelBuffers), std::end(pPixelBuffers), [](PixelBuffer* pPixelBuffer) { return pPixelBuffer != nullptr; });
	binaryFontBitmap.WriteUInt32(charactersCount);

	for (int i = 0; i < pixelBuffersCount; ++i)
	{
		PixelBuffer* pCharacterPixelBuffer = pPixelBuffers[i];
		if (pCharacterPixelBuffer == nullptr)
			continue;

		Data data;
		data.characterWidth = (float)pCharacterPixelBuffer->GetWidth();
		data.characterHeight = (float)pCharacterPixelBuffer->GetHeight();
		pConstantBuffer->UpdateData(&data);

		const float characterWidth = pCharacterPixelBuffer->GetWidth() * widthMultiplier;
		const float characterHeight = pCharacterPixelBuffer->GetHeight() * heightMultiplier;

		Viewport viewport;
		viewport.topLeftX = ((characterMaxWidth + characterPaddingX) * (i % charactersPerRow)) + ((characterMaxWidth - characterWidth) / 2.f);
		viewport.topLeftY = ((characterMaxHeight + characterPaddingY) * (i / charactersPerRow)) + ((characterMaxHeight - characterHeight) / 2.f);
		viewport.width = characterWidth;
		viewport.height = characterHeight;
		viewport.minDepth = 0.f;
		viewport.maxDepth = 1.f;
		pRenderer->SetViewport(viewport);

		char character = char(startCharacter + i);
		binaryFontBitmap.WriteChar(character);
		binaryFontBitmap.WriteFloat(viewport.topLeftX);
		binaryFontBitmap.WriteFloat(viewport.topLeftY);
		binaryFontBitmap.WriteFloat(viewport.width);
		binaryFontBitmap.WriteFloat(viewport.height);

		pRenderer->BindIndexBuffer(pIndexBuffer);
		pRenderer->BindVertexBuffer(pVertexBuffer);
		pRenderer->BindShaderState(pVertexShader);
		pRenderer->BindShaderState(pPixelShader);
		pRenderer->BindInputLayout(pInputLayout);
		pRenderer->BindReadBufferPS(pCharacterPixelBuffer, 0);
		pRenderer->BindConstantBufferPS(pConstantBuffer, 0);

		pRenderer->DrawIndexed(indicesCount);

		SAFE_DELETE(pCharacterPixelBuffer);
	}

	pFontBitmapPixelBuffer->SaveToBinaryFile(binaryFontBitmap);

	// ONLY ENABLE FOR DEBUGGING
	pFontBitmapPixelBuffer->SaveToTexture("C:\\Users\\Administrator\\Perforce\\STEFF-DESKTOP\\P3-Placeholder\\Dev\\data\\test\\test.png");
	// ONLY ENABLE FOR DEBUGGING

	SAFE_DELETE(pConstantBuffer);
	SAFE_DELETE(pInputLayout);
	SAFE_DELETE(pPixelShader);
	SAFE_DELETE(pVertexShader);
	SAFE_DELETE(pVertexBuffer);
	SAFE_DELETE(pIndexBuffer);
	SAFE_DELETE(pRenderTarget);

	SAFE_DELETE_ARRAY(pFileData);
#endif
}