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

inline p_int Random()
{
#if RAND_MAX == 0x7FFF // В этом случае rand() генерирует 15 случайных бит. Смещениями можно получить 32/64 бита.

#if SIZEOF_P_INT == 4
	return (rand() << 17) | (rand() << 2) | (rand() & 3);
#elif SIZEOF_P_INT == 8
	return (rand() << 49) | (rand() << 34) | (rand() << 19) | (rand() << 4) | (rand() & 15);
#else
#error Unknown SIZEOF_P_INT
#endif

#elif RAND_MAX == 0x7FFFFFFF

#if SIZEOF_P_INT == 4
	return (rand() << 1) | (rand() & 1);
#elif SIZEOF_P_INT == 8
	return (rand() << 33) | (rand() << 2) | (rand() & 3);
#else
#error Unknown SIZEOF_P_INT
#endif

#else
#error Unknown RAND_MAX
#endif
}

inline p_int Random(p_int max)
{
	return Random() % max;
}