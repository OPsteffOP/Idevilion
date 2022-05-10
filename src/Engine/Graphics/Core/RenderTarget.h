// Folder: Graphics/Core

#pragma once
#include "BufferEnums.h"

class PixelBuffer;

class RenderTarget
{
public:
	RenderTarget(PixelBuffer* pColorBuffer, PixelBuffer* pDepthStencilBuffer);

	virtual ~RenderTarget();

	PixelBuffer* GetColorBuffer() const { return m_pColorBuffer; }
	PixelBuffer* GetDepthStencilBuffer() const { return m_pDepthStencilBuffer; }

protected:
	PixelBuffer* m_pColorBuffer;
	PixelBuffer* m_pDepthStencilBuffer;
};