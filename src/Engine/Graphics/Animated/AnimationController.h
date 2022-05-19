// Folder: Graphics/Animated

#pragma once

class AnimationController
{
public:
	AnimationController() = default;
	AnimationController(float sourceWidth, float sourceHeight, uint sourceColumns, uint sourceRows, uint animationFrameStart, uint animationFrameCount, float animationDuration);

	bool Update();

	void SetData(float sourceWidth, float sourceHeight, uint sourceColumns, uint sourceRows, uint animationFrameStart, uint animationFrameCount, float animationDuration);

	const Rect4f& GetCurrentAnimationSourceRect() const { return m_CurrentFrameSourceRect; }

private:
	void NextFrame();
	void CalculateCurrentFrameSourceRect();

private:
	float m_NextFrameSeconds;
	float m_CurrentElapsedSeconds;

	uint m_FrameStart;
	uint m_FrameEnd;
	uint m_CurrentFrameIndex;

	float m_SourceWidth;
	float m_SourceHeight;
	uint m_SourceColumns;
	uint m_SourceRows;
	Rect4f m_CurrentFrameSourceRect;
};