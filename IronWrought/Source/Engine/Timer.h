#pragma once
#include <chrono>

class CTimer {
public:
	static float Time();
	static float Dt();
	static float FixedDt();

	CTimer();
	~CTimer();

	static float Mark();

	static bool FixedTimeStep();

private:
	float NewFrame();
	float TotalTime() const;

private:
	static CTimer* ourInstance;
	std::chrono::steady_clock::time_point myFirst;
	std::chrono::steady_clock::time_point myLast;
	float myDeltaTime;
	float myFixedTimer;
	float myFixedTimeInterval;
};