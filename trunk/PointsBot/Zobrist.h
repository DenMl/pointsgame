#pragma once

#include "BasicTypes.h"
#include "BasicConstants.h"

extern p_int Zobrist[2][2][PointsLength22];

void ZobristInit();

inline p_int GetZobristHash(p_int Player, p_int Surrounded, p_int Pos)
{
	return Zobrist[Player][Surrounded][Pos];
}

inline p_int GetZobristHash(p_int Pos)
{
	return Zobrist[0][0][Pos];
}