#include "Config.h"
#include "MinMaxEstimate.h"
#include "Field.h"
#include "TrajectoryList.h"
#include <omp.h>
#include <algorithm>
#include <vector>

using namespace std;

inline void GetPoints(TrajectoryList &Trajectories1, TrajectoryList &Trajectories2, uint TrajectoriesBoard[], vector<uint> &Moves)
{
	Trajectories1.ExcludeCompositeTrajectories();
	Trajectories2.ExcludeCompositeTrajectories();

	// ���������� ������������� ���������� �� �����.
	Trajectories1.Project(TrajectoriesBoard);
	Trajectories2.Project(TrajectoriesBoard);
	// ��������� �� ����������, � ������� ������� ����� ����� �����, ������������� ������ ��.
	while (Trajectories1.ExcludeUnnecessaryTrajectories(TrajectoriesBoard) || Trajectories2.ExcludeUnnecessaryTrajectories(TrajectoriesBoard));
	// �������� ������ �����, �������� � ���������� ������������� ����������.
	Moves.clear();
	Trajectories1.GetPoints(Moves);
	Trajectories2.GetPoints(Moves);

#if ALPHABETA_SORT
	// ��������� ����� �� �������� ���������� ����������, � ������� ��� ������.
	//ShellSort(Moves.Stack, TrajectoriesBoard, Moves.Count);
	sort(Moves.begin(), Moves.end(), [&](uint x, uint y){ return TrajectoriesBoard[x] < TrajectoriesBoard[y]; });
#endif

	// ������� ����� �� ��������.
	Trajectories1.UnProject(TrajectoriesBoard);
	Trajectories2.UnProject(TrajectoriesBoard);
	// ����� ��������� ������ ����� ������� �������� � ������������ ��� ���������� (��� ���������� ������ ��������).
	Trajectories1.IncludeAllTrajectories();
	Trajectories2.IncludeAllTrajectories(); //TODO: MAX_SPEED
}

// ����������� ������� ���������.
// CurField - ����, �� ������� ������� ����� ������� ����.
// TrajectoriesBoard - �����, �� ������� ������������ ����������. ������ ���� ��������� ������. ����� ��� �����������.
// Depth - ������� ��������.
// Pos - ��������� ���������, �� �� ��������� ���.
// alpha, beta - �������� ������, ��� �������� ������ ��� ������.
// �� ������ ������ ������� ��� CurPlayer (�� ���� Pos).
int Negamax(Field &CurField, uint TrajectoriesBoard[], uint Depth, uint Pos, TrajectoryList &LastEnemyTrajectories, TrajectoryList &LastCurrentTrajectories, int alpha, int beta)
{
	int BestEstimate = INT_MIN + 1;
	TrajectoryList Trajectories[2];

	// ������ ���, ��������� �� ���������� ������ ��������, ����� ���� ���� ��� ���������� ���������.
	CurField.DoUnsafeStep(Pos);

	if (Depth == 0)
	{
		BestEstimate = CurField.GetScore(CurField.CurPlayer);
		CurField.UndoStep();
		return -BestEstimate;
	}

	if (CurField.DCaptureCount < 0) // ���� ����� ���������� � ���������.
	{
		CurField.UndoStep();
		return INT_MIN + 1; // ��� CurPlayer ��� ������, �� ���� ������ Infinity.
	}

	if (Depth > 1)
		Trajectories[NextPlayer(CurField.CurPlayer)].BuildEnemyTrajectories(CurField, LastEnemyTrajectories, Pos, Depth / 2);
	Trajectories[CurField.CurPlayer].BuildCurrentTrajectories(CurField, LastCurrentTrajectories, Pos, (Depth + 1) / 2, CurField.CurPlayer);
	//Trajectories[CurField.CurPlayer].BuildTrajectories(CurField, (Depth + 1) / 2, CurField.CurPlayer);
	
	vector<uint> Moves;
	GetPoints(Trajectories[0], Trajectories[1], TrajectoriesBoard, Moves);

	for (vector<uint>::const_iterator i = Moves.begin(); i < Moves.end(); i++)
	{
		int CurEstimate = Negamax(CurField, TrajectoriesBoard, Depth - 1, *i, Trajectories[CurField.CurPlayer], Trajectories[NextPlayer(CurField.CurPlayer)], -beta, -alpha);
		if (CurEstimate > BestEstimate)
		{
			BestEstimate = CurEstimate;
			if (BestEstimate >= beta)
				break;
			if (BestEstimate > alpha)
				alpha = BestEstimate;
		}
	}
	if (BestEstimate == INT_MIN + 1)
		BestEstimate = CurField.GetScore(CurField.CurPlayer);

	CurField.UndoStep();

	return -BestEstimate;
}

int GetEnemyEstimate(Field &CurField, TrajectoryList &CurrentTrajectories, TrajectoryList &EnemyTrajectories, uint TrajectoriesBoard[], uint Depth)
{
	TrajectoryList TempTrajectories;
	vector<uint> Moves;
	int Result = INT_MIN + 1;

	TempTrajectories.BuildEnemyTrajectories(CurField, CurrentTrajectories, 0, (Depth + 1) / 2 - 1);
	GetPoints(EnemyTrajectories, TempTrajectories, TrajectoriesBoard, Moves);

	int alpha = INT_MIN + 1;
	CurField.SetNextPlayer();
	Depth -= 2;
	omp_lock_t lock;
	omp_init_lock(&lock);
	#pragma omp parallel
	{
		int MaxEstimate = INT_MIN + 1, CurEstimate;
		Field LocalField(CurField);
		uint TrajectoriesBoard[PointsLength22] = {0};

		#pragma omp for schedule(dynamic, 1)
		for (int i = 0; i < Moves.size(); i++)
		{
			CurEstimate = Negamax(LocalField, TrajectoriesBoard, Depth, Moves[i], EnemyTrajectories, CurrentTrajectories, INT_MIN + 1, -alpha);
			if (CurEstimate > alpha) // ��������� ������ �������.
				alpha = CurEstimate;
			if (CurEstimate > MaxEstimate)
				MaxEstimate = CurEstimate;
		}

		omp_set_lock(&lock);
		if (MaxEstimate > Result)
			Result = MaxEstimate;
		omp_unset_lock(&lock);
	}
	omp_destroy_lock(&lock);
	CurField.SetNextPlayer();

	EnemyTrajectories.IncludeAllTrajectories();

	return -Result;
}

// CurField - ����, �� ������� ������������ ������.
// Depth - ������� ������.
// Moves - �� ����� ��������� ����, �� ������ ������ �� ���.
int MinMaxEstimate(Field &CurField, uint Depth, vector<uint> &Moves)
{
	// ���������� � ����������� ��������� ������.
	int BestEstimate = INT_MIN + 1;
	// ������� ���������� - ���� � ���������.
	TrajectoryList Trajectories[2];
	// �����, �� ������� ���� ������������� ����������. ���������� ��� ����������� ������ � �������.
	uint TrajectoriesBoard[PointsLength22] = {0};
	vector<uint> BestMoves, PossibleMoves, FirstMoves;
	// ����� � �������� �����.
	int ScoreBoard[PointsLength22];

	// ������ ���-�� ������ ����� ������� �������� ������������� � ����������� ��������� ����� �� ����� �� ����� 0.
	if (Depth <= 0 || Moves.size() == 0)
		return 0;
	// ������ ���� ���������.
	Trajectories[CurField.CurPlayer].BuildTrajectories(CurField, (Depth + 1) / 2, CurField.CurPlayer);
	// ������ ���������� ����������.
	if (Depth > 1)
		Trajectories[NextPlayer(CurField.CurPlayer)].BuildTrajectories(CurField, Depth / 2, NextPlayer(CurField.CurPlayer));
	// �������� ���� �� ���������� (������� ����� ����� �������������), � ������� ����������� �� �������� ���������� �������.
	GetPoints(Trajectories[0], Trajectories[1], TrajectoriesBoard, PossibleMoves);
	for (vector<uint>::const_iterator i = PossibleMoves.begin(); i < PossibleMoves.end(); i++)
		if (find(Moves.begin(), Moves.end(), *i) != Moves.end())
			FirstMoves.push_back(*i);
	// ���� ��� ��������� �����, �������� � ���������� - �������.
	if (FirstMoves.size() == 0)
		return 0;
	// ��� ����� ���� ��������� �����, �� �������� � ���������� ������ ����� ����� ��, ��� ���� �� ����� CurPlayer ��������� ���. ���������� ������ ��� ���� �����, ��� ��� ����� ��� �����, ������� ������ � ���������� ��� �������������.
	int EnemyEstimate = GetEnemyEstimate(CurField, Trajectories[CurField.CurPlayer], Trajectories[NextPlayer(CurField.CurPlayer)], TrajectoriesBoard, Depth);
	for (vector<uint>::const_iterator i = Moves.begin(); i < Moves.end(); i++)
		ScoreBoard[*i] = EnemyEstimate;

	int alpha = INT_MIN + 2;
	#pragma omp parallel
	{
		Field LocalField(CurField);
		uint TrajectoriesBoard[PointsLength22] = {0};

		#pragma omp for schedule(dynamic, 1)
		for (int i = 0; i < FirstMoves.size(); i++)
		{
			int CurEstimate = Negamax(LocalField, TrajectoriesBoard, Depth - 1, FirstMoves[i], Trajectories[LocalField.CurPlayer], Trajectories[NextPlayer(LocalField.CurPlayer)], INT_MIN + 1, -alpha + 1);
			if (CurEstimate > alpha) // ��������� ������ �������.
				alpha = CurEstimate;
			ScoreBoard[FirstMoves[i]] = CurEstimate;
		}
	}

	for (vector<uint>::const_iterator i = Moves.begin(); i < Moves.end(); i++)
		if (ScoreBoard[*i] > BestEstimate)
		{
			BestEstimate = ScoreBoard[*i];
			BestMoves.clear();
			BestMoves.push_back(*i);
		}
		else if (ScoreBoard[*i] == BestEstimate)
		{
			BestMoves.push_back(*i);
		}
	Moves.assign(BestMoves.begin(), BestMoves.end());
	return BestEstimate;
}