#pragma once

#include "Config.h"
#include <utility>
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
struct point
{
	coord x, y;
};

// Правила обработки пустых баз.
// SurStandart = 0 - если PlayerRed ставит в пустую базу и ничего не обводит, то PlayerBlack обводит эту территорию.
// SurAlways = 1 - обводить базу, даже если нет вражеских точек внутри.
// SurAlwaysEnemy = 2 - обводит всегда PlayerBlack, если PlayerRed поставил точку в пустую базу.
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