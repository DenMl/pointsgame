#include "config.h"
#include "minimax.h"
#include "field.h"
#include "trajectories.h"
#include <omp.h>
#include <algorithm>
#include <limits>

using namespace std;

// Рекурсивная функция минимакса.
// CurField - поле, на котором ведется поиск лучшего хода.
// TrajectoriesBoard - доска, на которую проецируются траектории. Должна быть заполнена нулями. Нужна для оптимизации.
// Depth - глубина просчета.
// Pos - последний выбранный, но не сделанный ход.
// alpha, beta - интервал оценок, вне которого искать нет смысла.
// На выходе оценка позиции для CurPlayer (до хода Pos).
score alphabeta(field* cur_field, size_t depth, pos cur_pos, trajectories* last, score alpha, score beta, int* empty_board)
{
	trajectories cur_trajectories(*cur_field, empty_board);

	// Делаем ход, выбранный на предыдущем уровне рекурсии, после чего этот ход становится вражеским.
	cur_field->do_unsafe_step(cur_pos);

	if (depth == 0)
	{
		score best_estimate = cur_field->get_score(cur_field->get_player());
		cur_field->undo_step();
		return -best_estimate;
	}

	if (cur_field->get_d_score() < 0) // Если точка поставлена в окружение.
	{
		cur_field->undo_step();
		return -SCORE_INFINITY; // Для CurPlayer это хорошо, то есть оценка Infinity.
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

// CurField - поле, на котором производится оценка.
// Depth - глубина оценки.
// Moves - на входе возможные ходы, на выходе лучшие из них.
pos minimax(field* cur_field, size_t depth)
{
	// Главные траектории - свои и вражеские.
	trajectories cur_trajectories(*cur_field, NULL, depth);
	vector<pos> moves;
	pos result;

	// Делаем что-то только когда глубина просчета положительная и колическтво возможных ходов на входе не равно 0.
	if (depth <= 0)
		return -1;

	// Получаем ходы из траекторий (которые имеет смысл рассматривать), и находим пересечение со входными возможными точками.
	cur_trajectories.build_trajectories();
	cur_trajectories.get_points(moves);
	// Если нет возможных ходов, входящих в траектории - выходим.
	if (moves.size() == 0)
		return -1;
	// Для почти всех возможных точек, не входящих в траектории оценка будет такая же, как если бы игрок CurPlayer пропустил ход. Записываем оценку для всех ходов, так как потом для ходов, которые входят в траектории она перезапишется.
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
				if (cur_estimate > alpha) // Обновляем нижнюю границу.
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