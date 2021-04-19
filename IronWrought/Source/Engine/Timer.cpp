#include "stdafx.h"
#include "Timer.h"

CTimer* CTimer::ourInstance = nullptr;
CTimer::CTimer()
{
	ourInstance = this;
	myLast = std::chrono::steady_clock::now();
	myFirst = myLast;

	myFixedTimer = 0.0f;
	myFixedTimeInterval = 1.0f / 60.0f;
}
CTimer::~CTimer()
{
	ourInstance = nullptr;
}

//Total duration in seconds since start
float CTimer::Time()
{
	return ourInstance->TotalTime();
}

//Delta time in seconds between the last two frames
float CTimer::Dt()
{
	return ourInstance->myDeltaTime;
}

float CTimer::FixedDt()
{
	return ourInstance->myFixedTimeInterval;
}

float CTimer::Mark()
{
	return ourInstance->NewFrame();
}

bool CTimer::FixedTimeStep()
{
	return ourInstance->myFixedTimer >= ourInstance->myFixedTimeInterval;
}

float CTimer::NewFrame()
{
	const auto old = myLast;
	myLast = std::chrono::steady_clock::now();
	const std::chrono::duration<float> dt = myLast - old;
	myDeltaTime = dt.count();

	if (myFixedTimer > myFixedTimeInterval)
	{
		myFixedTimer -= myFixedTimeInterval;
	}
	
	myFixedTimer += myDeltaTime;

	return myDeltaTime;
}

float CTimer::TotalTime() const
{
	return std::chrono::duration<float>(std::chrono::steady_clock::now() - myFirst).count();
}