#pragma once

#include "config.h"
#include <utility>
#include <stack>
#include <climits>

using namespace std;

typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

typedef int pos;
typedef short value;
typedef short player;
typedef int score;
typedef short coord;

#define SCORE_INFINITY INT_MAX

// Структура координат точки.
struct point
{
	coord x, y;
};

// Правила обработки пустых баз.
// SC_STANDART = 0 - если PlayerRed ставит в пустую базу и ничего не обводит, то PlayerBlack обводит эту территорию.
// SC_ALWAYS = 1 - обводить базу, даже если нет вражеских точек внутри.
// SC_ALWAYS_ENEMY = 2 - обводит всегда PlayerBlack, если PlayerRed поставил точку в пустую базу.
enum sur_cond
{
	SC_STANDART,
	SC_ALWAYS,
	SC_ALWAYS_ENEMY
};

// Используемый шаблон в начале игры.
enum begin_pattern
{
	BP_CLEAN,
	BP_CROSSWIRE,
	BP_SQUARE
};

enum intersection_state
{
	IS_NONE,
	IS_UP,
	IS_DOWN,
	IS_TARGET
};

struct board_change
{
	uint capture_count[2];
	player player;
	ulong hash;
	stack<pair<pos, value>> changes;
};