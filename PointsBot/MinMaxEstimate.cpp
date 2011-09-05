#include "Config.h"
#include "MinMaxEstimate.h"
#include "Field.h"
#include "TrajectoryList.h"
#include "static_vector.h"
#include <omp.h>
#include <algorithm>
#include <climits>

using namespace std;

inline void get_points(TrajectoryList &trajectories1, TrajectoryList &trajectories2, uint trajectories_board[], static_vector<pos, MAX_CHAIN_POINTS> &moves)
{
	trajectories1.ExcludeCompositeTrajectories();
	trajectories2.ExcludeCompositeTrajectories();

	// ���������� ������������� ���������� �� �����.
	trajectories1.Project(trajectories_board);
	trajectories2.Project(trajectories_board);
	// ��������� �� ����������, � ������� ������� ����� ����� �����, ������������� ������ ��.
	while (trajectories1.ExcludeUnnecessaryTrajectories(trajectories_board) || trajectories2.ExcludeUnnecessaryTrajectories(trajectories_board));
	// �������� ������ �����, �������� � ���������� ������������� ����������.
	moves.clear();
	trajectories1.GetPoints(moves);
	trajectories2.GetPoints(moves);

#if ALPHABETA_SORT
	// ��������� ����� �� �������� ���������� ����������, � ������� ��� ������.
	sort(moves.begin(), moves.end(), [&](uint x, uint y){ return trajectories_board[x] < trajectories_board[y]; });
#endif

	// ������� ����� �� ��������.
	trajectories1.UnProject(trajectories_board);
	trajectories2.UnProject(trajectories_board);
	// ����� ��������� ������ ����� ������� �������� � ������������ ��� ���������� (��� ���������� ������ ��������).
	trajectories1.IncludeAllTrajectories();
	trajectories2.IncludeAllTrajectories();
}

// ����������� ������� ���������.
// CurField - ����, �� ������� ������� ����� ������� ����.
// TrajectoriesBoard - �����, �� ������� ������������ ����������. ������ ���� ��������� ������. ����� ��� �����������.
// Depth - ������� ��������.
// Pos - ��������� ���������, �� �� ��������� ���.
// alpha, beta - �������� ������, ��� �������� ������ ��� ������.
// �� ������ ������ ������� ��� CurPlayer (�� ���� Pos).
int negamax(field &cur_field, uint TrajectoriesBoard[], uint Depth, uint Pos, TrajectoryList &LastEnemyTrajectories, TrajectoryList &LastCurrentTrajectories, int alpha, int beta)
{
	int BestEstimate = INT_MIN + 1;
	TrajectoryList Trajectories[2];

	// ������ ���, ��������� �� ���������� ������ ��������, ����� ���� ���� ��� ���������� ���������.
	cur_field.do_unsafe_step(Pos);

	if (Depth == 0)
	{
		BestEstimate = cur_field.get_score(cur_field.get_player());
		cur_field.undo_step();
		return -BestEstimate;
	}

	if (cur_field.get_d_score() < 0) // ���� ����� ���������� � ���������.
	{
		cur_field.undo_step();
		return INT_MIN + 1; // ��� CurPlayer ��� ������, �� ���� ������ Infinity.
	}

	if (Depth > 1)
		Trajectories[next_player(cur_field.get_player())].BuildEnemyTrajectories(cur_field, LastEnemyTrajectories, Pos, Depth / 2);
	Trajectories[cur_field.get_player()].BuildCurrentTrajectories(cur_field, LastCurrentTrajectories, Pos, (Depth + 1) / 2, cur_field.get_player());
	//Trajectories[CurField.CurPlayer].BuildTrajectories(CurField, (Depth + 1) / 2, CurField.CurPlayer);
	
	static_vector<pos, MAX_CHAIN_POINTS> Moves;
	get_points(Trajectories[0], Trajectories[1], TrajectoriesBoard, Moves);

	for (auto i = Moves.begin(); i < Moves.end(); i++)
	{
		int CurEstimate = negamax(cur_field, TrajectoriesBoard, Depth - 1, *i, Trajectories[cur_field.get_player()], Trajectories[next_player(cur_field.get_player())], -beta, -alpha);
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
		BestEstimate = cur_field.get_score(cur_field.get_player());

	cur_field.undo_step();

	return -BestEstimate;
}

int get_enemy_estimate(field &cur_field, TrajectoryList &cur_trajectories, TrajectoryList &enemy_trajectories, size_t depth)
{
	TrajectoryList TempTrajectories;
	static_vector<pos, MAX_CHAIN_POINTS> Moves;

	uint* TrajectoriesBoard = new uint[cur_field.length()];
	fill_n(TrajectoriesBoard, cur_field.length(), 0);

	TempTrajectories.BuildEnemyTrajectories(cur_field, cur_trajectories, 0, (depth + 1) / 2 - 1);
	get_points(enemy_trajectories, TempTrajectories, TrajectoriesBoard, Moves);

	int alpha = INT_MIN + 1;
	cur_field.set_next_player();
	omp_lock_t lock;
	omp_init_lock(&lock);
	#pragma omp parallel
	{
		field LocalField(cur_field);
		uint TrajectoriesBoard[PointsLength22] = {0};

		#pragma omp for schedule(dynamic, 1)
		for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(Moves.size()); i++)
		{
			int CurEstimate = negamax(LocalField, TrajectoriesBoard, depth - 2, Moves[i], enemy_trajectories, cur_trajectories, INT_MIN + 1, -alpha);
			omp_set_lock(&lock);
			if (CurEstimate > alpha) // ��������� ������ �������.
				alpha = CurEstimate;
			omp_unset_lock(&lock);
		}
	}
	omp_destroy_lock(&lock);
	cur_field.set_next_player();

	enemy_trajectories.IncludeAllTrajectories();

	delete TrajectoriesBoard;

	return -alpha;
}

// CurField - ����, �� ������� ������������ ������.
// Depth - ������� ������.
// Moves - �� ����� ��������� ����, �� ������ ������ �� ���.
pos minmax(field &cur_field, size_t depth, list<pos> &moves)
{
	// ������� ���������� - ���� � ���������.
	TrajectoryList Trajectories[2];
	// �����, �� ������� ���� ������������� ����������. ���������� ��� ����������� ������ � �������.
	uint TrajectoriesBoard[PointsLength22] = {0};
	static_vector<pos, MAX_CHAIN_POINTS> BestMoves, PossibleMoves, FirstMoves;
	pos result;

	// ������ ���-�� ������ ����� ������� �������� ������������� � ����������� ��������� ����� �� ����� �� ����� 0.
	if (depth <= 0 || moves.size() == 0)
		return -1;
	// ������ ���� ���������.
	Trajectories[cur_field.get_player()].BuildTrajectories(cur_field, (depth + 1) / 2, cur_field.get_player());
	// ������ ���������� ����������.
	if (depth > 1)
		Trajectories[next_player(cur_field.get_player())].BuildTrajectories(cur_field, depth / 2, next_player(cur_field.get_player()));
	// �������� ���� �� ���������� (������� ����� ����� �������������), � ������� ����������� �� �������� ���������� �������.
	get_points(Trajectories[0], Trajectories[1], TrajectoriesBoard, PossibleMoves);
	for (auto i = PossibleMoves.begin(); i < PossibleMoves.end(); i++)
		if (find(moves.begin(), moves.end(), *i) != moves.end())
			FirstMoves.push_back(*i);
	// ���� ��� ��������� �����, �������� � ���������� - �������.
	if (FirstMoves.size() == 0)
		return 0;
	// ��� ����� ���� ��������� �����, �� �������� � ���������� ������ ����� ����� ��, ��� ���� �� ����� CurPlayer ��������� ���. ���������� ������ ��� ���� �����, ��� ��� ����� ��� �����, ������� ������ � ���������� ��� �������������.
	int enemy_estimate = get_enemy_estimate(cur_field, Trajectories[cur_field.get_player()], Trajectories[next_player(cur_field.get_player())], depth);

	omp_lock_t lock;
	omp_init_lock(&lock);
	int alpha = INT_MIN + 1;
	#pragma omp parallel
	{
		field local_field(cur_field);
		uint TrajectoriesBoard[PointsLength22] = {0};

		#pragma omp for schedule(dynamic, 1)
		for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(FirstMoves.size()); i++)
		{
			int cur_estimate = negamax(local_field, TrajectoriesBoard, depth - 1, FirstMoves[i], Trajectories[local_field.get_player()], Trajectories[next_player(local_field.get_player())], INT_MIN + 1, -alpha);
			omp_set_lock(&lock);
			if (cur_estimate > alpha) // ��������� ������ �������.
			{
				alpha = cur_estimate;
				result = FirstMoves[i];
			}
			omp_unset_lock(&lock);
		}
	}
	omp_destroy_lock(&lock);
	return result;
}