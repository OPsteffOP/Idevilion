// Folder: Utils/Time

#pragma once

class GameTime
{
public:
	static void Start();
	static void Update();

	static float GetTotalElapsedSec();
	static float GetElapsedSec() { return m_ElapsedSec; }

private:
	static std::chrono::high_resolution_clock::time_point m_StartTime;
	static std::chrono::high_resolution_clock::time_point m_StartFrameTime;

	static float m_ElapsedSec;
};