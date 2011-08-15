#pragma once

#include "Config.h"
#include <climits>

#if WINDOWS
#include <Windows.h>
#elif LINUX
#include <time.h>
#endif

using namespace std;

inline unsigned long GetTime()
{
#if WINDOWS
	return GetTickCount();
#elif LINUX
	timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
#else
#error Unknown OS
#endif
}

class Timer
{
private:
	ulong LastTime;

public:
	inline Timer()
	{
		Set();
	}

	inline void Set()
	{
		LastTime = GetTime();
	}

	inline ulong Get()
	{
		ulong CurTime = GetTime();
		if (CurTime > LastTime)
			return CurTime - LastTime;
		else
			return ULONG_MAX - LastTime + CurTime + 1;
	}
};