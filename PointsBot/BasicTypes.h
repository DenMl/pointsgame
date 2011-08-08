#pragma once

#include "Config.h"

typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

// Структура координат точки.
struct Point
{
	ushort X, Y;
};

template<typename T1, typename T2> struct Pair
{
	T1 first;
	T2 second;
};