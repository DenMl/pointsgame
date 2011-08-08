#include "Random.h"
#include "BasicTypes.h"
#include "BasicConstants.h"

ulong Zobrist[2][2][PointsLength22];

void ZobristInit()
{
	for (uint i = 0; i < PointsLength22; i++)
	{
		Zobrist[0][0][i] = Random();
		Zobrist[0][1][i] = Random();
		Zobrist[1][0][i] = Random();
		Zobrist[1][1][i] = Random();
	}
}