#include "BasicTypes.h"
#include "Config.h"
#include "AuxFunc.h"
#include "Field.h"

// Учет позиционных эвристик.
_int PositionEstimate(Field &MainField, _int TestedPoint, _int Player)
{
	const _int cgSumma[] = {-5, -1, 0, 0, 1, 2, 5, 20, 30};
	_int g1, g2;
	_int c1, c2;
	_int Result;

	g1 = MainField.NumberNearGroups(TestedPoint, Player);
	g2 = MainField.NumberNearGroups(TestedPoint, Field::NextPlayer(Player));
	c1 = cgSumma[MainField.NumberNearPoints(TestedPoint, Player)];
	c2 = cgSumma[MainField.NumberNearPoints(TestedPoint, Field::NextPlayer(Player))];
	Result = (g1 * 3 + g2 * 2) * (5 - Abs(g1 - g2)) - c1 - c2;
	if (MainField.PointsSeq.Count > 0)
		Result += Field::IsNear(MainField.PointsSeq.Stack[MainField.PointsSeq.Count - 1], TestedPoint) * 5;
	// Эмпирическая формула оценки важности точки при просчете ходов.
	return Result;
}

void PositionEstimate(Field &MainField, GameStack<_int, MAX_CHAIN_POINTS> &Moves)
{
	_int BestScore = -Infinity;
	GameStack<_int, MAX_CHAIN_POINTS> BestMoves;
	BestMoves.Clear();
	for (_int i = 0; i < Moves.Count; i++)
	{
		_int TempScore = PositionEstimate(MainField, Moves.Stack[i], MainField.CurPlayer);
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
	if (BestMoves.Count != 0)
		Moves.Copy(BestMoves);
}