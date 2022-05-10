// Folder: Graphics/Text

#pragma once
#include "Viewport.h"

class Buffer;
class PixelBuffer;
class ShaderState;
class InputLayout;
class Sampler;

class Font
{
public:
	explicit Font(const std::string& bpfFilePath);
	~Font();

	float GetTextWidth(const std::string& text, uint fontSize);

	void DrawText(const std::string& text, uint fontSize, const Color& textColor, const Point2f& destPosition, bool isScreenSpace = true);
	void DrawText(const std::string& text, uint fontSize, const Color& textColor, const Rect4f& destRect, bool isScreenSpace = true);

private:
	void CreateBuffers();

	void DrawText(const std::string& text, uint fontSize, const Color& textColor, const Viewport& viewport);

private:
	std::unordered_map<char, Rect4f> m_CharacterData;
	PixelBuffer* m_pPixelBuffer;

	static uint m_IndexCount;
	static Buffer* m_pIndexBuffer;
	static Buffer* m_pVertexBuffer;
	static ShaderState* m_pVertexShader;
	static InputLayout* m_pInputLayout;
	static Sampler* m_pDefaultSampler;
	static Buffer* m_pCharacterConstantBuffer;
	static Buffer* m_pTextColorConstantBuffer;

	static ShaderState* m_pDefaultPixelShader;
	ShaderState* m_pPixelShader;
};