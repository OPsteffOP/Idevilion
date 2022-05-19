// Folder: Graphics/Animated

#include "EnginePCH.h"
#include "AnimationController.h"

AnimationController::AnimationController(float sourceWidth, float sourceHeight, uint sourceColumns, uint sourceRows, uint animationFrameStart, uint animationFrameCount, float animationDuration)
{
	SetData(sourceWidth, sourceHeight, sourceColumns, sourceRows, animationFrameStart, animationFrameCount, animationDuration);
}

bool AnimationController::Update()
{
	const float deltaSeconds = GameTime::GetElapsedSec();

	m_CurrentElapsedSeconds += deltaSeconds;
	if (m_CurrentElapsedSeconds >= m_NextFrameSeconds)
	{
		m_CurrentElapsedSeconds -= m_NextFrameSeconds;
		NextFrame();
		return true;
	}

	return false;
}

void AnimationController::SetData(float sourceWidth, float sourceHeight, uint sourceColumns, uint sourceRows, uint animationFrameStart, uint animationFrameCount, float animationDuration)
{
	m_NextFrameSeconds = animationDuration / animationFrameCount;
	m_CurrentElapsedSeconds = 0.f;

	m_FrameStart = animationFrameStart;
	m_FrameEnd = animationFrameStart + animationFrameCount - 1;
	m_CurrentFrameIndex = animationFrameStart;

	m_SourceWidth = sourceWidth;
	m_SourceHeight = sourceHeight;
	m_SourceColumns = sourceColumns;
	m_SourceRows = sourceRows;

	CalculateCurrentFrameSourceRect();
}

void AnimationController::NextFrame()
{
	++m_CurrentFrameIndex;
	if (m_CurrentFrameIndex > m_FrameEnd)
		m_CurrentFrameIndex = m_FrameStart;

	CalculateCurrentFrameSourceRect();
}

void AnimationController::CalculateCurrentFrameSourceRect()
{
	const float animationFrameWidth = m_SourceWidth / m_SourceColumns;
	const float animationFrameHeight = m_SourceHeight / m_SourceRows;

	m_CurrentFrameSourceRect.x = animationFrameWidth * (m_CurrentFrameIndex % m_SourceColumns);
	m_CurrentFrameSourceRect.y = animationFrameHeight * (m_CurrentFrameIndex / m_SourceColumns);
	m_CurrentFrameSourceRect.width = animationFrameWidth;
	m_CurrentFrameSourceRect.height = animationFrameHeight;
}