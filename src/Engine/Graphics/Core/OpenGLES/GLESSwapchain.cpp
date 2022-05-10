// Folder: Graphics/Core/OpenGLES

#include "EnginePCH.h"
#include "GLESSwapchain.h"

#ifdef BP_PLATFORM_ANDROID

#include "GLESRenderer.h"

GLESSwapchain::GLESSwapchain(uint width, uint height, WindowHandle windowHandle)
	: Swapchain(width, height, windowHandle)
	, m_pSwapchain(nullptr)
{
	EGLBoolean status;

	GLESRenderer* pRenderer = static_cast<GLESRenderer*>(Renderer::GetRenderer());

	const EGLint attribs[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_NONE
	};

	EGLConfig config;
	EGLint numConfigs;

	status = eglChooseConfig(pRenderer->m_Display, attribs, &config, 1, &numConfigs);
	if (status != EGL_TRUE || numConfigs == 0)
	{
		LOG_ERROR("Failed to retrieve OpenGLES config");
		return;
	}

	EGLint format;

	status = eglGetConfigAttrib(pRenderer->m_Display, config, EGL_NATIVE_VISUAL_ID, &format);
	if (status != EGL_TRUE)
	{
		LOG_ERROR("Failed to retrieve the OpenGLES EGL_NATIVE_VISUAL_ID config attribute");
		return;
	}

	// m_pDefaultRenderTarget = new D3D11RenderTarget(pColorPixelBuffer, pDepthStencilBuffer);
}

D3D11Swapchain::~D3D11Swapchain()
{
	SAFE_DELETE(m_pDefaultRenderTarget);
}

void D3D11Swapchain::Present()
{

}

#endif