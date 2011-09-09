#include "config.h"
#include "minimax.h"
#include "field.h"
#include "trajectories.h"
#include <omp.h>
#include <algorithm>
#include <climits>

using namespace std;

// Рекурсивная функция минимакса.
// CurField - поле, на котором ведется поиск лучшего хода.
// TrajectoriesBoard - доска, на которую проецируются траектории. Должна быть заполнена нулями. Нужна для оптимизации.
// Depth - глубина просчета.
// Pos - последний выбранный, но не сделанный ход.
// alpha, beta - интервал оценок, вне которого искать нет смысла.
// На выходе оценка позиции для CurPlayer (до хода Pos).
score negamax(field &cur_field, uint depth, uint cur_pos, trajectories &last, int alpha, int beta)
{
	trajectories cur_trajectories(cur_field);

	// Делаем ход, выбранный на предыдущем уровне рекурсии, после чего этот ход становится вражеским.
	cur_field.do_unsafe_step(cur_pos);

	if (depth == 0)
	{
		score best_estimate = cur_field.get_score(cur_field.get_player());
		cur_field.undo_step();
		return -best_estimate;
	}

	if (cur_field.get_d_score() < 0) // Если точка поставлена в окружение.
	{
		cur_field.undo_step();
		return -SCORE_INFINITY; // Для CurPlayer это хорошо, то есть оценка Infinity.
	}

	cur_trajectories.build_trajectories(last, cur_pos);
	
	vector<pos> Moves;
	cur_trajectories.get_points(Moves);

	if (Moves.size() == 0)
	{
		score best_estimate = cur_field.get_score(cur_field.get_player());
		cur_field.undo_step();
		return -best_estimate;
	}

	for (auto i = Moves.begin(); i < Moves.end(); i++)
	{
		score cur_estimate = negamax(cur_field, depth - 1, *i, cur_trajectories, -beta, -alpha);
		if (cur_estimate > alpha)
		{
			alpha = cur_estimate;
			if (alpha >= beta)
				break;
		}
	}

	cur_field.undo_step();
	return -alpha;
}

// int get_enemy_estimate(field &cur_field, TrajectoryList &cur_trajectories, TrajectoryList &enemy_trajectories, size_t depth)
// {
// 	TrajectoryList TempTrajectories;
// 	static_vector<pos, MAX_CHAIN_POINTS> Moves;
// 
// 	uint* TrajectoriesBoard = new uint[cur_field.length()];
// 	fill_n(TrajectoriesBoard, cur_field.length(), 0);
// 
// 	TempTrajectories.BuildEnemyTrajectories(cur_field, cur_trajectories, 0, (depth + 1) / 2 - 1);
// 	get_points(enemy_trajectories, TempTrajectories, TrajectoriesBoard, Moves);
// 
// 	int alpha = INT_MIN + 1;
// 	cur_field.set_next_player();
// 	omp_lock_t lock;
// 	omp_init_lock(&lock);
// 	#pragma omp parallel
// 	{
// 		field LocalField(cur_field);
// 		uint TrajectoriesBoard[PointsLength22] = {0};
// 
// 		#pragma omp for schedule(dynamic, 1)
// 		for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(Moves.size()); i++)
// 		{
// 			int CurEstimate = negamax(LocalField, TrajectoriesBoard, depth - 2, Moves[i], enemy_trajectories, cur_trajectories, INT_MIN + 1, -alpha);
// 			omp_set_lock(&lock);
// 			if (CurEstimate > alpha) // Обновляем нижнюю границу.
// 				alpha = CurEstimate;
// 			omp_unset_lock(&lock);
// 		}
// 	}
// 	omp_destroy_lock(&lock);
// 	cur_field.set_next_player();
// 
// 	enemy_trajectories.IncludeAllTrajectories();
// 
// 	delete TrajectoriesBoard;
// 
// 	return -alpha;
// }

// CurField - поле, на котором производится оценка.
// Depth - глубина оценки.
// Moves - на входе возможные ходы, на выходе лучшие из них.
pos minimax(field &cur_field, size_t depth, list<pos> &moves)
{
	// Главные траектории - свои и вражеские.
	trajectories cur_trajectories(cur_field, depth);
	vector<pos> BestMoves, PossibleMoves, FirstMoves;
	pos result;

	// Делаем что-то только когда глубина просчета положительная и колическтво возможных ходов на входе не равно 0.
	if (depth <= 0 || moves.size() == 0)
		return -1;

	// Получаем ходы из траекторий (которые имеет смысл рассматривать), и находим пересечение со входными возможными точками.
	cur_trajectories.build_trajectories();
	cur_trajectories.get_points(PossibleMoves);
	for (auto i = PossibleMoves.begin(); i < PossibleMoves.end(); i++)
		if (find(moves.begin(), moves.end(), *i) != moves.end())
			FirstMoves.push_back(*i);
	// Если нет возможных ходов, входящих в траектории - выходим.
	if (FirstMoves.size() == 0)
		return -1;
	// Для почти всех возможных точек, не входящих в траектории оценка будет такая же, как если бы игрок CurPlayer пропустил ход. Записываем оценку для всех ходов, так как потом для ходов, которые входят в траектории она перезапишется.
	//int enemy_estimate = get_enemy_estimate(cur_field, Trajectories[cur_field.get_player()], Trajectories[next_player(cur_field.get_player())], depth);

	omp_lock_t lock;
	omp_init_lock(&lock);
	score alpha = -SCORE_INFINITY;
	#pragma omp parallel
	{
		field local_field(cur_field);

		#pragma omp for schedule(dynamic, 1)
		for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(FirstMoves.size()); i++)
		{
			int cur_estimate = negamax(local_field, depth - 1, FirstMoves[i], cur_trajectories, -SCORE_INFINITY, -alpha);
			omp_set_lock(&lock);
			if (cur_estimate > alpha) // Обновляем нижнюю границу.
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