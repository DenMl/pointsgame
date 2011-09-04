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

// ��������� ��������� �����.
struct point
{
	coord x, y;
};

// ������� ��������� ������ ���.
// SurStandart = 0 - ���� PlayerRed ������ � ������ ���� � ������ �� �������, �� PlayerBlack ������� ��� ����������.
// SurAlways = 1 - �������� ����, ���� ���� ��� ��������� ����� ������.
// SurAlwaysEnemy = 2 - ������� ������ PlayerBlack, ���� PlayerRed �������� ����� � ������ ����.
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