#include "Config.h"
#include "MinMaxEstimate.h"
#include "Field.h"
#include "TrajectoryList.h"
#include <omp.h>

// inc[] - ������, � ������� ��������� ����������.
// size - ����������� ����� �������.
p_int increment(p_int inc[], p_int size)
{
	p_int p1 = 1, p2 = 1, p3 = 1, s = -1;

	// ��������� ������ ��������� �� ������� ������� ��������.
	do
	{
		if (++s % 2)
		{
			inc[s] = 8*p1 - 6*p2 + 1;
		}
		else
		{
			inc[s] = 9*p1 - 9*p3 + 1;
			p2 *= 2;
			p3 *= 2;
		}
		p1 *= 2;
	} while (3*inc[s] < size); // ��������� ������, ���� ������� ���������� ���� �� � 3 ���� ������ ���������� ��������� � �������.

	// ���������� ���������� ��������� � �������.
	return s > 0 ? --s : 0;
}
void ShellSort(p_int a[], p_int values[], p_int size)
{
	// inc - ���������, ���������� ����� ���������� ���������.
	// i � j - ����������� ���������� �����.
	// seq[40] - ������, � ������� �������� ����������.
	p_int inc, j, seq[40];

	// ���������� ������������������ ����������.
	p_int s = increment(seq, size);
	while (s >= 0)
	{
		// ��������� �� ������� ��������� ����������.
		inc = seq[s--];

		// ���������� ��������� � ������������ inc[].
		for (p_int i = inc; i < size; i++)
		{
			p_int temp = a[i];
			// �������� �������� �� ��� ���, ���� �� ������ �� ����� ��� �� ���������� � ������ �������.
			for (j = i-inc; (j >= 0) && (values[a[j]] < values[temp]); j -= inc)
				a[j+inc] = a[j];
			// ����� ���� ������� ������ �� ����� j+inc �������, ������� ��������� �� i �����.
			a[j+inc] = temp;
		}
	}
}

inline void GetPoints(TrajectoryList &Trajectories1, TrajectoryList &Trajectories2, p_int TrajectoriesBoard[], GameStack<p_int, MAX_CHAIN_POINTS> &Moves)
{
	Trajectories1.ExcludeCompositeTrajectories();
	Trajectories2.ExcludeCompositeTrajectories();

	// ���������� ������������� ���������� �� �����.
	Trajectories1.Project(TrajectoriesBoard);
	Trajectories2.Project(TrajectoriesBoard);
	// ��������� �� ����������, � ������� ������� ����� ����� �����, ������������� ������ ��.
	while (Trajectories1.ExcludeUnnecessaryTrajectories(TrajectoriesBoard) || Trajectories2.ExcludeUnnecessaryTrajectories(TrajectoriesBoard));
	// �������� ������ �����, �������� � ���������� ������������� ����������.
	Moves.Clear();
	Trajectories1.GetPoints(Moves);
	Trajectories2.GetPoints(Moves);

#if ALPHABETA_SORT
	// ��������� ����� �� �������� ���������� ����������, � ������� ��� ������.
	ShellSort(Moves.Stack, TrajectoriesBoard, Moves.Count);
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
p_int Negamax(Field &CurField, p_int TrajectoriesBoard[], p_int Depth, p_int Pos, TrajectoryList &LastEnemyTrajectories, TrajectoryList &LastCurrentTrajectories, p_int alpha, p_int beta)
{
	p_int BestEstimate = -INFINITY;
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
		return -INFINITY; // ��� CurPlayer ��� ������, �� ���� ������ Infinity.
	}

	if (Depth > 1)
		Trajectories[CurField.EnemyPlayer].BuildEnemyTrajectories(CurField, LastEnemyTrajectories, Pos, Depth / 2);
	Trajectories[CurField.CurPlayer].BuildCurrentTrajectories(CurField, LastCurrentTrajectories, Pos, (Depth + 1) / 2, CurField.CurPlayer);
	//Trajectories[CurField.CurPlayer].BuildTrajectories(CurField, (Depth + 1) / 2, CurField.CurPlayer);
	
	GameStack<p_int, MAX_CHAIN_POINTS> Moves;
	GetPoints(Trajectories[0], Trajectories[1], TrajectoriesBoard, Moves);

	for (p_int i = 0; i < Moves.Count; i++)
	{
		p_int CurEstimate = Negamax(CurField, TrajectoriesBoard, Depth - 1, Moves.Stack[i], Trajectories[CurField.CurPlayer], Trajectories[CurField.EnemyPlayer], -beta, -alpha);
		if (CurEstimate > BestEstimate)
		{
			BestEstimate = CurEstimate;
			if (BestEstimate >= beta)
				break;
			if (BestEstimate > alpha)
				alpha = BestEstimate;
		}
	}
	if (BestEstimate == -INFINITY)
		BestEstimate = CurField.GetScore(CurField.CurPlayer);

	CurField.UndoStep();

	return -BestEstimate;
}

p_int GetEnemyEstimate(Field &CurField, TrajectoryList &CurrentTrajectories, TrajectoryList &EnemyTrajectories, p_int TrajectoriesBoard[], p_int Depth)
{
	TrajectoryList TempTrajectories;
	GameStack<p_int, MAX_CHAIN_POINTS> Moves;
	p_int Result = 0;

	TempTrajectories.BuildEnemyTrajectories(CurField, CurrentTrajectories, 0, (Depth + 1) / 2 - 1);
	GetPoints(EnemyTrajectories, TempTrajectories, TrajectoriesBoard, Moves);

	p_int alpha = -INFINITY;
	CurField.SetNextPlayer();
	Depth -= 2;
	omp_lock_t lock;
	omp_init_lock(&lock);
	#pragma omp parallel
	{
		p_int MaxEstimate = -INFINITY, CurEstimate;
		Field* LocalField = new Field(CurField);
		p_int TrajectoriesBoard[PointsLength22] = {0};

		#pragma omp for schedule(dynamic, 1)
		for (p_int i = 0; i < Moves.Count; i++)
		{
			CurEstimate = Negamax(*LocalField, TrajectoriesBoard, Depth, Moves.Stack[i], EnemyTrajectories, CurrentTrajectories, -INFINITY, -alpha);
			if (CurEstimate > alpha) // ��������� ������ �������.
				alpha = CurEstimate;
			if (CurEstimate > MaxEstimate)
				MaxEstimate = CurEstimate;
		}

		delete LocalField;

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
p_int MinMaxEstimate(Field &CurField, p_int Depth, GameStack<p_int, MAX_CHAIN_POINTS> &Moves)
{
	// ���������� � ����������� ��������� ������.
	p_int BestEstimate = -INFINITY;
	// ������� ���������� - ���� � ���������.
	TrajectoryList Trajectories[2];
	// �����, �� ������� ���� ������������� ����������. ���������� ��� ����������� ������ � �������.
	p_int TrajectoriesBoard[PointsLength22] = {0};
	GameStack<p_int, MAX_CHAIN_POINTS> PotentialMoves, OutMoves;
	// ����� � �������� �����.
	p_int ScoreBoard[PointsLength22];

	// ������ ���-�� ������ ����� ������� �������� ������������� � ����������� ��������� ����� �� ����� �� ����� 0.
	if (Depth <= 0 || Moves.Count == 0)
		return 0;
	// ������ ���� ���������.
	Trajectories[CurField.CurPlayer].BuildTrajectories(CurField, (Depth + 1) / 2, CurField.CurPlayer);
	// ������ ���������� ����������.
	if (Depth > 1)
		Trajectories[CurField.EnemyPlayer].BuildTrajectories(CurField, Depth / 2, CurField.EnemyPlayer);
	// �������� ���� �� ���������� (������� ����� ����� �������������), � ������� ����������� �� �������� ���������� �������.
	GetPoints(Trajectories[0], Trajectories[1], TrajectoriesBoard, PotentialMoves);
	PotentialMoves.Intersect(Moves);
	// ���� ��� ��������� �����, �������� � ���������� - �������.
	if (PotentialMoves.Count == 0)
		return 0;
	// ��� ����� ���� ��������� �����, �� �������� � ���������� ������ ����� ����� ��, ��� ���� �� ����� CurPlayer ��������� ���. ���������� ������ ��� ���� �����, ��� ��� ����� ��� �����, ������� ������ � ���������� ��� �������������.
	p_int EnemyEstimate = GetEnemyEstimate(CurField, Trajectories[CurField.CurPlayer], Trajectories[CurField.EnemyPlayer], TrajectoriesBoard, Depth);
	for (p_int i = 0; i < Moves.Count; i++)
		ScoreBoard[Moves.Stack[i]] = EnemyEstimate;

	p_int alpha = -(INFINITY - 1);
	#pragma omp parallel
	{
		Field* LocalField = new Field(CurField);
		p_int TrajectoriesBoard[PointsLength22] = {0};

		#pragma omp for schedule(dynamic, 1)
		for (p_int i = 0; i < PotentialMoves.Count; i++)
		{
			p_int CurEstimate = Negamax(*LocalField, TrajectoriesBoard, Depth - 1, PotentialMoves.Stack[i], Trajectories[LocalField->CurPlayer], Trajectories[LocalField->EnemyPlayer], -INFINITY, -alpha + 1);
			if (CurEstimate > alpha) // ��������� ������ �������.
				alpha = CurEstimate;
			ScoreBoard[PotentialMoves.Stack[i]] = CurEstimate;
		}

		delete LocalField;
	}

	for (p_int i = 0; i < Moves.Count; i++)
		if (ScoreBoard[Moves.Stack[i]] > BestEstimate)
		{
			BestEstimate = ScoreBoard[Moves.Stack[i]];
			OutMoves.Clear();
			OutMoves.Push(Moves.Stack[i]);
		}
		else if (ScoreBoard[Moves.Stack[i]] == BestEstimate)
		{
			OutMoves.Push(Moves.Stack[i]);
		}
	Moves.Copy(OutMoves);
	return BestEstimate;
}