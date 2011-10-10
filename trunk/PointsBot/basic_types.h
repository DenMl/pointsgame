#pragma once

#include "config.h"
#include <utility>
#include <stack>
#include <climits>

using namespace std;

typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

typedef size_t pos;
typedef char value;
typedef short player;
typedef int score;
typedef short coord;
typedef size_t hash_t;

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
// BP_CLEAN - начало с чистого поля.
// BP_CROSSWIRE - начало со скреста.
// BP_SQUARE - начало с квадрата.
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

// Одно изменение доски.
struct board_change
{
	// Предыдущий счет захваченных точек.
	score capture_count[2];
	// Предыдущий игрок.
	player player;
	// Предыдущий хеш.
	hash_t hash;
	// Список изменных точек (координата - значение до изменения).
	stack<pair<pos, value>> changes;
};