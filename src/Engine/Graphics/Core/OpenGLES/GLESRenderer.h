// Folder: Graphics/Core/OpenGLES

#pragma once
#include "Renderer.h"

#ifdef BP_PLATFORM_ANDROID

class GLESRenderer : public Renderer
{
	friend class GLESSwapchain;
	//friend class D3D11RenderTarget;
	//friend class D3D11InputLayout;
	//friend class D3D11Buffer;
	//friend class D3D11PixelBuffer;
	//friend class D3D11ShaderState;
	//friend class D3D11Sampler;

public:
	GLESRenderer();

	virtual void Initialize(RendererFlags flags) override;

protected:
	EGLDisplay m_Display;
};

#endif