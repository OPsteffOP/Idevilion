// Folder: Graphics/Core

#include "EnginePCH.h"
#include "Renderer.h"

Renderer* Renderer::m_pInstance = nullptr;

Renderer::Renderer(RenderAPI api)
	: m_CurrentAPI(api)
{}

void Renderer::SetRenderer(Renderer* pRenderer)
{
	if (m_pInstance != nullptr)
	{
		LOG_ERROR("Trying to create a renderer while one already exists");
		return;
	}

	m_pInstance = pRenderer;
}

Renderer* Renderer::GetRenderer()
{
	if (m_pInstance == nullptr)
		LOG_ERROR("Trying to get the current renderer while none has been created");

	return m_pInstance;
}

void Renderer::Destroy()
{
	SAFE_DELETE(m_pInstance);
}