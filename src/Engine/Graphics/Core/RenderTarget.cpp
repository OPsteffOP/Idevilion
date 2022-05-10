// Folder: Graphics/Core

#include "EnginePCH.h"
#include "RenderTarget.h"

#include "PixelBuffer.h"

RenderTarget::RenderTarget(PixelBuffer* pColorBuffer, PixelBuffer* pDepthStencilBuffer)
	: m_pColorBuffer(pColorBuffer)
	, m_pDepthStencilBuffer(pDepthStencilBuffer)
{
	// TODO: VALIDATE BUFFERS
}

RenderTarget::~RenderTarget()
{
	SAFE_DELETE(m_pColorBuffer);
	SAFE_DELETE(m_pDepthStencilBuffer);
}