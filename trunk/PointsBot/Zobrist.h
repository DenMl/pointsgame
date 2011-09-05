#pragma once

#include "config.h"
#include "basic_types.h"

using namespace std;

extern ulong Zobrist[2][2][2000];

void ZobristInit();

inline ulong GetZobristHash(short Player, short Surrounded, uint Pos)
{
	return Zobrist[Player][Surrounded][Pos];
}

inline ulong GetZobristHash(uint Pos)
{
	return Zobrist[0][0][Pos];
}