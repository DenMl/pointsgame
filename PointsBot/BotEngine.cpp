#include "BasicTypes.h"
#include "Field.h"
#include "Config.h"
#include "BotEngine.h"
#include "MinMaxEstimate.h"
#include "UCTEstimate.h"
#include "PositionEstimate.h"
#include "LadderEstimate.h"
#include "Random.h"

void BuildAllMoves(Field &MainField, GameStack<p_int, MAX_CHAIN_POINTS> &Moves)
{
	Moves.Clear();
	for (p_int i = MainField.MinPos; i <= MainField.MaxPos; i++)
		if (MainField.PuttingAllow(i))
			Moves.Push(i);
}

p_int SearchBestMove(Field &MainField, p_int Depth, p_int UCTIterations)
{
	p_int MaxScore = -INFINITY;

	GameStack<p_int, MAX_CHAIN_POINTS> Moves;
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