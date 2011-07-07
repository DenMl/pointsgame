#pragma once

#include "BasicTypes.h"
#include "BasicConstants.h"

extern _int Zobrist[2][2][PointsLength22];

void ZobristInit();

inline _int GetZobristHash(_int Player, _int Surrounded, _int Pos)
{
	return Zobrist[Player][Surrounded][Pos];
}

inline _int GetZobristHash(_int Pos)
{
	return Zobrist[0][0][Pos];
}