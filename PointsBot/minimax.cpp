#include "config.h"
#include "minimax.h"
#include "field.h"
#include "trajectories.h"
#include <omp.h>
#include <algorithm>
#include <limits>

using namespace std;

// ����������� ������� ���������.
// CurField - ����, �� ������� ������� ����� ������� ����.
// TrajectoriesBoard - �����, �� ������� ������������ ����������. ������ ���� ��������� ������. ����� ��� �����������.
// Depth - ������� ��������.
// Pos - ��������� ���������, �� �� ��������� ���.
// alpha, beta - �������� ������, ��� �������� ������ ��� ������.
// �� ������ ������ ������� ��� CurPlayer (�� ���� Pos).
score alphabeta(field* cur_field, size_t depth, pos cur_pos, trajectories* last, score alpha, score beta, int* empty_board)
{
	trajectories cur_trajectories(*cur_field, empty_board);

	// ������ ���, ��������� �� ���������� ������ ��������, ����� ���� ���� ��� ���������� ���������.
	cur_field->do_unsafe_step(cur_pos);

	if (depth == 0)
	{
		score best_estimate = cur_field->get_score(cur_field->get_player());
		cur_field->undo_step();
		return -best_estimate;
	}

	if (cur_field->get_d_score() < 0) // ���� ����� ���������� � ���������.
	{
		cur_field->undo_step();
		return -SCORE_INFINITY; // ��� CurPlayer ��� ������, �� ���� ������ Infinity.
	}

	cur_trajectories.build_trajectories(*last, cur_pos);
	
	vector<pos> moves;
	cur_trajectories.get_points(moves);

	if (moves.size() == 0)
	{
		score best_estimate = cur_field->get_score(cur_field->get_player());
		cur_field->undo_step();
		return -best_estimate;
	}

	for (auto i = moves.begin(); i < moves.end(); i++)
	{
		score cur_estimate = alphabeta(cur_field, depth - 1, *i, &cur_trajectories, -beta, -alpha, empty_board);
		if (cur_estimate > alpha)
		{
			alpha = cur_estimate;
			if (alpha >= beta)
				break;
		}
	}

	cur_field->undo_step();
	return -alpha;
}

// CurField - ����, �� ������� ������������ ������.
// Depth - ������� ������.
// Moves - �� ����� ��������� ����, �� ������ ������ �� ���.
pos minimax(field* cur_field, size_t depth)
{
	// ������� ���������� - ���� � ���������.
	trajectories cur_trajectories(*cur_field, NULL, depth);
	vector<pos> moves;
	pos result;

	// ������ ���-�� ������ ����� ������� �������� ������������� � ����������� ��������� ����� �� ����� �� ����� 0.
	if (depth <= 0)
		return -1;

	// �������� ���� �� ���������� (������� ����� ����� �������������), � ������� ����������� �� �������� ���������� �������.
	cur_trajectories.build_trajectories();
	cur_trajectories.get_points(moves);
	// ���� ��� ��������� �����, �������� � ���������� - �������.
	if (moves.size() == 0)
		return -1;
	// ��� ����� ���� ��������� �����, �� �������� � ���������� ������ ����� ����� ��, ��� ���� �� ����� CurPlayer ��������� ���. ���������� ������ ��� ���� �����, ��� ��� ����� ��� �����, ������� ������ � ���������� ��� �������������.
	//int enemy_estimate = get_enemy_estimate(cur_field, Trajectories[cur_field.get_player()], Trajectories[next_player(cur_field.get_player())], depth);

	score alpha = -SCORE_INFINITY;
	#pragma omp parallel
	{
		field* local_field = new field(*cur_field);
		int* empty_board = new int[cur_field->length()];

		#pragma omp for schedule(dynamic, 1)
		for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(moves.size()); i++)
		{
			int cur_estimate = alphabeta(local_field, depth - 1, moves[i], &cur_trajectories, -SCORE_INFINITY, -alpha, empty_board);
			#pragma omp critical
			{
				if (cur_estimate > alpha) // ��������� ������ �������.
				{
					alpha = cur_estimate;
					result = moves[i];
				}
			}
		}

		delete empty_board;
		delete local_field;
	}
	return result;
}