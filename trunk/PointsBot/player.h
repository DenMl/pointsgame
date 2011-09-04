#pragma once

#include "config.h"

using namespace std;

// Значение бита первого игрока.
const short player_red = 0x0;
// Значение бита второго игрока.
const short player_black = 0x1;

// Получить по игроку следующего игрока.
inline short next_player(const short cur_player)
{
	return cur_player ^ 1;
}