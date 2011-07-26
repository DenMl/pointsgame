#pragma once

#include "Config.h"
#include "BasicTypes.h"
#include <random>

#if WINDOWS
#include <Windows.h>
#else
#include <ctime>
#endif

// ������������ ������������������ ��������������� �����. ������ ���������� �������� ��� ������� ������, ���� ��������� ������.
inline void Randomize()
{
#if WINDOWS
	srand(GetTickCount());
#else
	srand(time(0));
#endif
}

// ���������� ��������� p_int (��� ���� ��������� ��������� ��������).
inline p_int Random()
{
#if RAND_MAX == 0x7FFF // � ���� ������ rand() ���������� 15 ��������� ���. ���������� ����� �������� 32/64 ����.

#if SIZEOF_P_INT == 4
	return (rand() << 17) | (rand() << 2) | (rand() & 3);
#elif SIZEOF_P_INT == 8
	return (rand() << 49) | (rand() << 34) | (rand() << 19) | (rand() << 4) | (rand() & 15);
#else
#error Unknown SIZEOF_P_INT
#endif

#elif RAND_MAX == 0x7FFFFFFF // � ���� ������ rand() ���������� 31 ��������� ���. ���������� ����� �������� 32/64 ����.

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

// ���������� ��������� ����� �� 0 �� max-1. ���� ���������� ��������� �����, �� ������������� RAND_MAX, ����� ������������ FastRandom.
inline p_int Random(p_int max)
{
	return Random() % max;
}

// ���������� ��������� ����� �� 0 �� max-1. max <= RAND_MAX.
inline p_int FastRandom(p_int max)
{
	return rand() % max;
}