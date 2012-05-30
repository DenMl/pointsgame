#pragma once

#include "config.h"
#include <utility>
#include <stack>
#include <boost/random.hpp>

using namespace std;
using namespace boost;

typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

typedef size_t pos;
typedef short value;
typedef short player;
typedef int score;
typedef short coord;
typedef size_t hash_t;

#define SCORE_INFINITY numeric_limits<score>::max()

// Структура координат точки.
struct point
{
	coord x, y;
};

// Используемый шаблон в начале игры.
// BP_CLEAN - начало с чистого поля.
// BP_CROSSWIRE - начало со скреста.
// BP_SQUARE - начало с квадрата.
enum begin_pattern
{
	BEGIN_PATTERN_CLEAN,
	BEGIN_PATTERN_CROSSWIRE,
	BEGIN_PATTERN_SQUARE
};

enum intersection_state
{
	INTERSECTION_STATE_NONE,
	INTERSECTION_STATE_UP,
	INTERSECTION_STATE_DOWN,
	INTERSECTION_STATE_TARGET
};

// Одно изменение доски.
struct board_change
{
	// Предыдущий счет захваченных точек.
	score capture_count[2];
	// Предыдущий игрок.
	player last_player;
	// Предыдущий хеш.
	hash_t hash;
	// Список изменных точек (координата - значение до изменения).
	stack<pair<pos, value>> changes;
};

#if ENVIRONMENT_32
typedef random::mt19937 mt;
#elif ENVIRONMENT_64
typedef random::mt19937_64 mt;
#endif