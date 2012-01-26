#pragma once

#include "config.h"
#include <utility>
#include <stack>
#include <climits>
#include <boost/random.hpp>

using namespace std;
using namespace boost;

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

// ��������� ��������� �����.
struct point
{
	coord x, y;
};

// ������������ ������ � ������ ����.
// BP_CLEAN - ������ � ������� ����.
// BP_CROSSWIRE - ������ �� �������.
// BP_SQUARE - ������ � ��������.
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

// ���� ��������� �����.
struct board_change
{
	// ���������� ���� ����������� �����.
	score capture_count[2];
	// ���������� �����.
	player last_player;
	// ���������� ���.
	hash_t hash;
	// ������ �������� ����� (���������� - �������� �� ���������).
	stack<pair<pos, value>> changes;
};

#if X64
typedef random::mt19937_64 mt;
#else
typedef random::mt19937 mt;
#endif