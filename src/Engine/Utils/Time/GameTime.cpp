// Folder: Utils/Time

#include "EnginePCH.h"
#include "GameTime.h"

std::chrono::high_resolution_clock::time_point GameTime::m_StartTime;
std::chrono::high_resolution_clock::time_point GameTime::m_StartFrameTime;
float GameTime::m_ElapsedSec;

void GameTime::Start()
{
	m_StartTime = std::chrono::high_resolution_clock::now();
	m_StartFrameTime = m_StartTime;
}

void GameTime::Update()
{
	const std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
	m_ElapsedSec = std::chrono::duration<float>(now - m_StartFrameTime).count();
	m_StartFrameTime = now;
}

float GameTime::GetTotalElapsedSec()
{
	const std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
	return std::chrono::duration<float>(now - m_StartTime).count();
}