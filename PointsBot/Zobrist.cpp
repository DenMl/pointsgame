#include "random.h"
#include "basic_types.h"

using namespace std;

ulong Zobrist[2][2][2000];

void ZobristInit()
{
	for (uint i = 0; i < 2000; i++)
	{
		Zobrist[0][0][i] = Random();
		Zobrist[0][1][i] = Random();
		Zobrist[1][0][i] = Random();
		Zobrist[1][1][i] = Random();
	}
}