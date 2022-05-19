// Folder: Scene/Components/Core

#pragma once
#include "RenderComponent.h"

#include "AnimationController.h"

class AnimationRenderComponent : public RenderComponent
{
public:
	AnimationRenderComponent(const std::string& texturePath, uint sourceFrameColumns, uint sourceFrameRows, uint frameStart, uint frameCount, float frameDuration, float destWidth, float destHeight);
	AnimationRenderComponent(PixelBuffer* pPixelBuffer, uint sourceFrameColumns, uint sourceFrameRows, uint frameStart, uint frameCount, float frameDuration, float destWidth, float destHeight);

	virtual void Initialize() override;

	virtual void Update() override;

private:
	void SetSourceRect(const Rect4f& sourceRect, float textureWidth, float textureHeight);

private:
	AnimationController m_AnimationController;
};