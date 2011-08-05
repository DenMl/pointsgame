#pragma once

#include "Config.h"

#if WINDOWS
#include <Windows.h>
#elif LINUX
#include <time.h>
#endif

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