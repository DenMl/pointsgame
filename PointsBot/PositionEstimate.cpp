#include "BasicTypes.h"
#include "Config.h"
#include "AuxFunc.h"
#include "Player.h"
#include "Field.h"
#include "GameStack.h"
#include <limits>

using namespace std;

// Учет позиционных эвристик.
short PositionEstimate(Field &MainField, uint TestedPoint, short Player)
{
	const short cgSumma[] = {-5, -1, 0, 0, 1, 2, 5, 20, 30};
	short g1, g2;
	short c1, c2;
	short Result;

	g1 = MainField.NumberNearGroups(TestedPoint, Player);
	g2 = MainField.NumberNearGroups(TestedPoint, NextPlayer(Player));
	c1 = cgSumma[MainField.NumberNearPoints(TestedPoint, Player)];
	c2 = cgSumma[MainField.NumberNearPoints(TestedPoint, NextPlayer(Player))];
	Result = (g1 * 3 + g2 * 2) * (5 - Abs(g1 - g2)) - c1 - c2;
	if (MainField.PointsSeq->size() > 0 && Field::IsNear(MainField.PointsSeq->back(), TestedPoint))
		Result += 5;
	// Эмпирическая формула оценки важности точки при просчете ходов.
	return Result;
}

void PositionEstimate(Field &MainField, GameStack<uint, MAX_CHAIN_POINTS> &Moves)
{
	short BestScore = SHRT_MIN;
	GameStack<uint, MAX_CHAIN_POINTS> BestMoves;
	for (uint i = 0; i < Moves.Count; i++)
	{
		short TempScore = PositionEstimate(MainField, Moves.Stack[i], MainField.CurPlayer);
		if (TempScore > BestScore)
		{
			BestScore = TempScore;
			BestMoves.Clear();
			BestMoves.Push(Moves.Stack[i]);
		}
		else if (TempScore == BestScore)
		{
			BestMoves.Push(Moves.Stack[i]);
		}
	}
	Moves.Copy(BestMoves);
}