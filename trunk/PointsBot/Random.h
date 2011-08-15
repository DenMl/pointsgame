#pragma once

#include "Config.h"
#include "BasicTypes.h"
#include <random>
#include <climits>

#if WINDOWS
#include <Windows.h>
#else
#include <ctime>
#endif

using namespace std;

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
inline ulong Random()
{
#if RAND_MAX == 0x7FFF // � ���� ������ rand() ���������� 15 ��������� ���. ���������� ����� �������� 32/64 ����.

#if ULONG_MAX == 0xFFFFFFFFUL
	return (rand() << 17) | (rand() << 2) | (rand() & 3);
#elif ULONG_MAX == 0xFFFFFFFFFFFFFFFFUL
	return (rand() << 49) | (rand() << 34) | (rand() << 19) | (rand() << 4) | (rand() & 15);
#else
#error Unknown ULONG_MAX
#endif

#elif RAND_MAX == 0x7FFFFFFF // � ���� ������ rand() ���������� 31 ��������� ���. ���������� ����� �������� 32/64 ����.

#if ULONG_MAX == 0xFFFFFFFFUL
	return (rand() << 1) | (rand() & 1);
#elif ULONG_MAX == 0xFFFFFFFFFFFFFFFFUL
	return (rand() << 33) | (rand() << 2) | (rand() & 3);
#else
#error Unknown ULONG_MAX
#endif

#else
#error Unknown RAND_MAX
#endif
}