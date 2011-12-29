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

// ��������� ��������� �����.
struct point
{
	coord x, y;
};

// ������� ��������� ������ ���.
// SC_STANDART = 0 - ���� PlayerRed ������ � ������ ���� � ������ �� �������, �� PlayerBlack ������� ��� ����������.
// SC_ALWAYS = 1 - �������� ����, ���� ���� ��� ��������� ����� ������.
// SC_ALWAYS_ENEMY = 2 - ������� ������ PlayerBlack, ���� PlayerRed �������� ����� � ������ ����.
enum sur_cond
{
	SUR_COND_STANDART,
	SUR_COND_ALWAYS,
	SUR_COND_ALWAYS_ENEMY
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
	player player;
	// ���������� ���.
	hash_t hash;
	// ������ �������� ����� (���������� - �������� �� ���������).
	stack<pair<pos, value>> changes;
};