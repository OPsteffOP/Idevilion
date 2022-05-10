// Folder: Graphics/Core/OpenGLES

#include "EnginePCH.h"
#include "GLESRenderer.h"

#ifdef BP_PLATFORM_ANDROID

GLESRenderer::GLESRenderer()
	: Renderer(RenderAPI::OPENGL_ES)
{}

void GLESRenderer::Initialize(RendererFlags flags)
{
	EGLBoolean status;

	m_Display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (m_Display == EGL_NO_DISPLAY)
	{
		LOG_ERROR("Failed to retrieve OpenGLES display");
		return;
	}

	status = eglInitialize(m_Display, nullptr, nullptr);
	if (status != EGL_TRUE)
	{
		LOG_ERROR("Failed to initialize OpenGLES display");
		return;
	}
}

#endif