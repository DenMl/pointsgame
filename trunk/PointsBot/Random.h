#pragma once

#include "Config.h"
#include "BasicTypes.h"
#include <random>
#include <limits>

#if WINDOWS
#include <Windows.h>
#else
#include <ctime>
#endif

// Рандомизация последовательности псевдослучайных чисел. Должно вызываться отдельно для каждого потока, если необходим рандом.
inline void Randomize()
{
#if WINDOWS
	srand(GetTickCount());
#else
	srand(time(0));
#endif
}

// Генерирует случайное p_int (все биты принимают случайное значение).
inline ulong Random()
{
#if RAND_MAX == 0x7FFF // В этом случае rand() генерирует 15 случайных бит. Смещениями можно получить 32/64 бита.

#if ULONG_MAX == 0xFFFFFFFFUL
	return ((ulong)rand() << 17) | ((ulong)rand() << 2) | ((ulong)rand() & 3);
#elif ULONG_MAX == 0xFFFFFFFFFFFFFFFFUL
	return ((ulong)rand() << 49) | ((ulong)rand() << 34) | ((ulong)rand() << 19) | ((ulong)rand() << 4) | ((ulong)rand() & 15);
#else
#error Unknown ULONG_MAX
#endif

#elif RAND_MAX == 0x7FFFFFFF // В этом случае rand() генерирует 31 случайный бит. Смещениями можно получить 32/64 бита.

#if ULONG_MAX == 0xFFFFFFFFUL
	return ((ulong)rand() << 1) | ((ulong)rand() & 1);
#elif ULONG_MAX == 0xFFFFFFFFFFFFFFFFUL
	return ((ulong)rand() << 33) | ((ulong)rand() << 2) | ((ulong)rand() & 3);
#else
#error Unknown ULONG_MAX
#endif

#else
#error Unknown RAND_MAX
#endif
}