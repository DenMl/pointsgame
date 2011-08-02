#pragma once
#include "BasicTypes.h"

// Значение бита первого игрока.
static const p_int PlayerRed = 0x0;
// Значение бита второго игрока.
static const p_int PlayerBlack = 0x1;

// Получить по игроку следующего игрока.
inline p_int NextPlayer(const p_int Player)
{
	return Player ^ 1;
}