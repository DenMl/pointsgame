#pragma once

#include "Config.h"
#include "BasicTypes.h"
#include <random>

#if WINDOWS
#include <Windows.h>
#else
#include <ctime>
#endif

inline void Randomize()
{
#if WINDOWS
	srand(GetTickCount());
#else
	srand(time(0));
#endif
}

inline _int Random()
{
#if RAND_MAX == 0x7FFF // В этом случае rand() генерирует 15 случайных бит. Смещениями можно получить 32/64 бита.

#if X32
	return (rand() << 17) | (rand() << 2) | (rand() & 3);
#elif X64
	return (rand() << 49) | (rand() << 34) | (rand() << 19) | (rand() << 4) | (rand() & 15);
#endif

#elif RAND_MAX == 0x7FFFFFFF

#if X32
	return (rand() << 1) | (rand() & 1);
#elif X64
	return (rand() << 33) | (rand() << 2) | (rand() & 3);
#endif

#else
	Error!!! //TODO: Написать корректный генератор.
#endif
}

inline _int Random(_int max)
{
	return Random() % max;
}