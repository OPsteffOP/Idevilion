// Folder: Graphics/UI

#pragma once
#include "WindowManager.h"
#include "Viewport.h"

class UIElement;

class RenderTarget;
class Buffer;
class PixelBuffer;
class ShaderState;
class InputLayout;
class Sampler;

class RMUISystem
{
public:
	static RMUISystem* GetSystemForCurrentWindow();
	~RMUISystem();

	void Update();
	void Render();

	void AddElement(UIElement* pElement);
	void RemoveElement(UIElement* pElement, bool shouldDelete = true);

	void MarkDirty() { m_IsDirty = true; }

private:
	RMUISystem();

	void CreateBuffers();

private:
	static std::unordered_map<WindowIdentifier, RMUISystem*> m_pSystems;

	std::vector<UIElement*> m_pRootElements;

	Viewport m_Viewport;

	bool m_IsDirty;
	PixelBuffer* m_pRetainedPixelBuffer;
	RenderTarget* m_pRetainedRenderTarget;

	static uint m_IndexCount;
	static Buffer* m_pIndexBuffer;
	static Buffer* m_pVertexBuffer;
	static ShaderState* m_pVertexShader;
	static ShaderState* m_pPixelShader;
	static InputLayout* m_pInputLayout;
	static Sampler* m_pSampler;
};