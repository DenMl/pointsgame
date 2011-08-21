#pragma once

#include "Config.h"
#include <stack>

using namespace std;

typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

typedef int pos;
typedef short value;
typedef short player;
typedef int score;
typedef short coord;

// Структура координат точки.
struct Point
{
	coord X, Y;
};

enum SurroundCondition
{
	Standart,
	Always,
	AlwaysEnemy
};

enum BeginPattern
{
	CleanPattern,
	CrosswisePattern,
	SquarePattern
};

enum IntersectionState
{
	ISNone,
	ISUp,
	ISDown,
	ISTarget
};

template<typename T1, typename T2> struct Pair
{
	T1 first;
	T2 second;

	inline Pair(T1 a, T2 b)
	{
		first = a;
		second = b;
	}
};

struct BoardChange
{
	uint CaptureCount[2];
	player Player;
	ulong Hash;
	stack<Pair<pos, value>> Changes;
};