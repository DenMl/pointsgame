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
	SC_STANDART,
	SC_ALWAYS,
	SC_ALWAYS_ENEMY
};

// ������������ ������ � ������ ����.
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