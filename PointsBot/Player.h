#pragma once
#include "BasicTypes.h"

// Значение бита первого игрока.
static const short PlayerRed = 0x0;
// Значение бита второго игрока.
static const short PlayerBlack = 0x1;

// Получить по игроку следующего игрока.
inline short NextPlayer(const short Player)
{
	return Player ^ 1;
}