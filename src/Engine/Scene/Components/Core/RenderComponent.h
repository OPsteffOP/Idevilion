// Folder: Scene/Components/Core

#pragma once
#include "Component.h"

class Buffer;
class PixelBuffer;
class ShaderState;
class InputLayout;
class Sampler;

class RenderComponent : public Component
{
public:
	RenderComponent(const std::string& texturePath, float destWidth, float destHeight);
	RenderComponent(const std::string& texturePath, float destWidth, float destHeight, const Rect4f& srcRect);
	RenderComponent(PixelBuffer* pPixelBuffer, float destWidth, float destHeight, const Rect4f& srcRect);

	virtual ~RenderComponent() override;

	virtual void Initialize() override;

	virtual void Update() override;
	virtual void Render() override;

	PixelBuffer* GetTexture() const { return m_pTexture; }
	const Rect4f& GetSourceRect() const { return m_SrcRect; }
	float GetDestWidth() const { return m_DestWidth; }
	float GetDestHeight() const { return m_DestHeight; }

protected:
	struct Vertex
	{
		Point2f position;
		Point2f texCoord;
	};

	static uint m_IndexCount;
	static Buffer* m_pIndexBuffer;
	static Buffer* m_pVertexBuffer;
	static ShaderState* m_pDefaultVertexShader;
	static ShaderState* m_pDefaultPixelShader;
	static InputLayout* m_pDefaultInputLayout;
	static Sampler* m_pDefaultSampler;

	ShaderState* m_pVertexShader;
	ShaderState* m_pPixelShader;
	InputLayout* m_pInputLayout;
	Sampler* m_pSampler;

	PixelBuffer* m_pTexture;
	Buffer* m_pConstantBuffer;

	Rect4f m_SrcRect;
	float m_DestWidth;
	float m_DestHeight;

	bool m_ShouldForceUpdate;

private:
	void CreateBuffers();
};