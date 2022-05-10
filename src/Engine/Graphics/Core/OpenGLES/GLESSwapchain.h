// Folder: Graphics/Core/OpenGLES

#pragma once
#include "Swapchain.h"

#ifdef BP_PLATFORM_ANDROID

class GLESSwapchain : public Swapchain
{
	friend class D3D11Renderer;

public:
	GLESSwapchain(uint width, uint height, WindowHandle windowHandle);

	virtual ~GLESSwapchain() override;

	virtual void Present() override;

protected:

};

#endif