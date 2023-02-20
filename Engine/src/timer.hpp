#pragma once

#include <chrono>


class Timer
{
	std::chrono::time_point<std::chrono::steady_clock> recordedTimePoint;
	std::chrono::time_point<std::chrono::steady_clock> startTimePoint;

public:
	Timer() = default;

	void init()
	{
		startTimePoint = std::chrono::high_resolution_clock::now();
	}

	inline float elapsed()
	{
		return std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::high_resolution_clock::now() - recordedTimePoint).count();
	}

	inline void recordTime()
	{
		recordedTimePoint = std::chrono::high_resolution_clock::now();
	}

	inline float getTimePoint()
	{
		return std::chrono::duration_cast<std::chrono::duration<float>>(recordedTimePoint - startTimePoint).count();
	}
};