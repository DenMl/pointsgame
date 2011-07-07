#include "BasicTypes.h"
#include "Field.h"
#include "Config.h"
#include "BotEngine.h"
#include "MinMaxEstimate.h"
#include "UCTEstimate.h"
#include "PositionEstimate.h"
#include "LadderEstimate.h"
#include "Random.h"

void BuildAllMoves(Field &MainField, GameStack<_int, MAX_CHAIN_POINTS> &Moves)
{
	Moves.Clear();
	for (_int i = MainField.MinPos; i <= MainField.MaxPos; i++)
		if (MainField.PuttingAllow(i))
			Moves.Push(i);
}

_int SearchBestMove(Field &MainField, _int Depth, _int UCTIterations)
{
	_int MaxScore = -Infinity;

	GameStack<_int, MAX_CHAIN_POINTS> Moves;
	BuildAllMoves(MainField, Moves);

	// ���� �� ����� �� ����� �� ����� ����� - ���������� ��������� ���.
	if (MainField.PointsSeq.Count == 0)
		return Moves.Stack[rand() % Moves.Count];

	if (Moves.Count == 1) // ���� ��������� ��� ���� - ���������� ���.
		return Moves.Stack[0];
	else if (Moves.Count == 0) // ���� ��� ��������� ����� - ���������� ������ -1.
		return -1;

	MinMaxEstimate(MainField, Depth, Moves);

	if (Moves.Count == 1) // ���� ��������� ��� ���� - ���������� ���.
		return Moves.Stack[0];
	else if (Moves.Count == 0) // ���� ��� ��������� ����� - ���������� ������ -1.
		return -1;

	UCTEstimate(MainField, UCTIterations, Moves);

	if (Moves.Count == 1) // ���� ��������� ��� ���� - ���������� ���.
		return Moves.Stack[0];
	else if (Moves.Count == 0) // ���� ��� ��������� ����� - ���������� ������ -1.
		return -1;

	PositionEstimate(MainField, Moves);

	if (Moves.Count == 1) // ���� ��������� ��� ���� - ���������� ���.
		return Moves.Stack[0];
	else if (Moves.Count == 0) // ���� ��� ��������� ����� - ���������� ������ -1.
		return -1;
	else // ���� � ����� ��������� ��������� ������ ����� - ���������� ���������.
		return Moves.Stack[rand() % Moves.Count];
}