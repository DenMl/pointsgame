#pragma once

#include "config.h"

using namespace std;

// �������� ���� ������� ������.
const short player_red = 0x0;
// �������� ���� ������� ������.
const short player_black = 0x1;

// �������� �� ������ ���������� ������.
inline short next_player(const short cur_player)
{
	return cur_player ^ 1;
}