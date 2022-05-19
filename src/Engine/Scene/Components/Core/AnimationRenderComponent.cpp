// Folder: Scene/Components/Core

#include "EnginePCH.h"
#include "AnimationRenderComponent.h"

#include "PixelBuffer.h"

AnimationRenderComponent::AnimationRenderComponent(const std::string& texturePath, uint sourceFrameColumns, uint sourceFrameRows, uint frameStart, uint frameCount, float frameDuration, float destWidth, float destHeight)
	: RenderComponent(texturePath, destWidth, destHeight)
	, m_AnimationController()
{
	m_AnimationController.SetData((float)m_pTexture->GetWidth(), (float)m_pTexture->GetHeight(), sourceFrameColumns, sourceFrameRows, frameStart, frameCount, frameDuration);
}

AnimationRenderComponent::AnimationRenderComponent(PixelBuffer* pPixelBuffer, uint sourceFrameColumns, uint sourceFrameRows, uint frameStart, uint frameCount, float frameDuration, float destWidth, float destHeight)
	: RenderComponent(pPixelBuffer, destWidth, destHeight, Rect4f(0.f, 0.f, 1.f, 1.f))
	, m_AnimationController()
{
	m_AnimationController.SetData((float)m_pTexture->GetWidth(), (float)m_pTexture->GetHeight(), sourceFrameColumns, sourceFrameRows, frameStart, frameCount, frameDuration);
}

void AnimationRenderComponent::Initialize()
{
	SetSourceRect(m_AnimationController.GetCurrentAnimationSourceRect(), (float)m_pTexture->GetWidth(), (float)m_pTexture->GetHeight());
	RenderComponent::Initialize();
}

void AnimationRenderComponent::Update()
{
	const bool animationFrameChanged = m_AnimationController.Update();
	if (animationFrameChanged)
	{
		SetSourceRect(m_AnimationController.GetCurrentAnimationSourceRect(), (float)m_pTexture->GetWidth(), (float)m_pTexture->GetHeight());
		m_ShouldForceUpdate = true;
	}

	RenderComponent::Update();
}

void AnimationRenderComponent::SetSourceRect(const Rect4f& sourceRect, float textureWidth, float textureHeight)
{
	m_SrcRect.x = sourceRect.x / textureWidth;
	m_SrcRect.y = sourceRect.y / textureHeight;
	m_SrcRect.width = sourceRect.width / textureWidth;
	m_SrcRect.height = sourceRect.height / textureHeight;
}