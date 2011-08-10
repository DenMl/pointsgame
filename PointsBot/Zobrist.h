#pragma once

#include "Config.h"
#include "BasicTypes.h"
#include "BasicConstants.h"

extern ulong Zobrist[2][2][PointsLength22];

void ZobristInit();

inline ulong GetZobristHash(short Player, short Surrounded, uint Pos)
{
	return Zobrist[Player][Surrounded][Pos];
}

inline ulong GetZobristHash(uint Pos)
{
	return Zobrist[0][0][Pos];
}