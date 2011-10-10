#pragma once

#include "config.h"
#include "basic_types.h"

using namespace std;

// Значение бита первого игрока.
const player player_red = 0x0;
// Значение бита второго игрока.
const player player_black = 0x1;

// Получить по игроку следующего игрока.
inline player next_player(const player cur_player)
{
	return cur_player ^ 1;
}