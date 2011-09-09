#pragma once

#include "field.h"
#include "trajectory.h"
#include <list>
#include <vector>
#include <algorithm>

class trajectories
{
private:
	size_t _depth[2];
	field* _field;
	list<trajectory> _trajectories[2];
	int* _trajectories_board;
	zobrist* _zobrist;

private:
	template<typename _InIt>
	inline void add_trajectory(_InIt begin, _InIt end, player cur_player)
	{
		size_t cur_hash = 0;

		// Эвристические проверки.
		// Каждая точка траектории должна окружать что-либо и иметь рядом хотя бы 2 группы точек.
		// Если нет - не добавляем эту траекторию.
		for (auto i = begin; i < end; i++)
			if (!_field->is_base_bound(*i) || (_field->number_near_groups(*i, cur_player) < 2))
				return;

		// Высчитываем хеш траектории и сравниваем с уже существующими для исключения повторов.
		for (auto i = begin; i < end; i++)
			cur_hash ^= _zobrist->get_hash(*i);
		for (auto i = _trajectories[cur_player].begin(); i != _trajectories[cur_player].end(); i++)
			if (cur_hash == i->hash())
				return; // В теории возможны коллизии. Неплохо было бы сделать точную проверку.

		_trajectories[cur_player].push_back(trajectory());

		for (auto i = begin; i < end; i++)
		{
			// Добавляем точку в PointsSeq траектории.
			_trajectories[cur_player].back().push_back(*i, _zobrist->get_hash(*i));
		}
	}
	inline void add_trajectory(trajectory &cur_trajectory, player cur_player)
	{
		_trajectories[cur_player].push_back(cur_trajectory);
	}
	inline void add_trajectory(trajectory &cur_trajectory, pos cur_pos, player cur_player)
	{
		if (cur_trajectory.size() == 1)
			return;

		_trajectories[cur_player].push_back(trajectory());
		for (auto i = cur_trajectory.begin(); i != cur_trajectory.end(); i++)
			if (*i != cur_pos)
				_trajectories[cur_player].back().push_back(*i, _zobrist->get_hash(*i));
	}

	void build_trajectories_recursive(size_t depth, player cur_player)
	{
		for (pos Pos = _field->min_pos(); Pos <= _field->max_pos(); Pos++)
		{
			if (_field->putting_allow(Pos) && _field->is_near_points(Pos, cur_player))
			{
				if (_field->is_in_empty_base(Pos)) // Если поставили в пустую базу (свою или нет), то дальше строить траекторию нет нужды.
				{
					_field->do_unsafe_step(Pos, cur_player);
					if (_field->get_d_score() > 0)
						add_trajectory(_field->points_seq.end() - (_depth[cur_player] - depth), _field->points_seq.end(), cur_player);
					_field->undo_step();
				}
				else
				{
					_field->do_unsafe_step(Pos, cur_player);

#if SURROUND_CONDITIONS
					if (_field->is_base_bound(Pos) && _field->get_d_score() == 0)
					{
						_field->undo_step();
						continue;
					}
#endif

					if (_field->get_d_score() > 0)
						add_trajectory(_field->points_seq.end() - (_depth[cur_player] - depth), _field->points_seq.end(), cur_player);
					else if (depth > 0)
						build_trajectories_recursive(depth - 1, cur_player);

					_field->undo_step();
				}
			}
		}
	}
	inline void project(trajectory &cur_trajectory)
	{
		for (auto j = cur_trajectory.begin(); j != cur_trajectory.end(); j++)
			_trajectories_board[*j]++;
	}
	// Проецирует траектории на доску TrajectoriesBoard (для каждой точки Pos очередной траектории инкрементирует TrajectoriesBoard[Pos]).
	// Для оптимизации в данной реализации функции не проверяются траектории на исключенность (поле Excluded).
	inline void project(player cur_player)
	{
		for (auto i = _trajectories[cur_player].begin(); i != _trajectories[cur_player].end(); i++)
			if (!i->excluded())
				project(*i);
	}
	inline void project()
	{
		project(player_red);
		project(player_black);
	}
	inline void unproject(trajectory &cur_trajectory)
	{
		for (auto j = cur_trajectory.begin(); j != cur_trajectory.end(); j++)
			_trajectories_board[*j]--;
	}
	// Удаляет проекцию траекторий с доски TrajectoriesBoard.
	inline void unproject(player cur_player)
	{
		for (auto i = _trajectories[cur_player].begin(); i != _trajectories[cur_player].end(); i++)
			if (!i->excluded())
				unproject(*i);
	}
	inline void unproject()
	{
		unproject(player_red);
		unproject(player_black);
	}
	inline void include_all_trajectories(player cur_player)
	{
		for (auto i = _trajectories[cur_player].begin(); i != _trajectories[cur_player].end(); i++)
			i->include();
	}
	inline void include_all_trajectories()
	{
		include_all_trajectories(player_red);
		include_all_trajectories(player_black);
	}
	// Возвращает хеш Зобриста пересечения двух траекторий.
	inline ulong get_intersect_hash(trajectory &t1, trajectory &t2)
	{
		ulong result_hash = t1.hash();
		for (auto i = t2.begin(); i != t2.end(); i++)
			if (find(t1.begin(), t1.end(), *i) == t1.end())
				result_hash ^= _zobrist->get_hash(*i);
		return result_hash;
	}
	bool exclude_unnecessary_trajectories(player cur_player)
	{
		uint c;
		bool need_exclude = false;

		for (auto i = _trajectories[cur_player].begin(); i != _trajectories[cur_player].end(); i++)
		{
			if (i->excluded())
				continue;
			// Считаем в c количество точек, входящих только в эту траекторию.
			c = 0;
			for (auto j = i->begin(); j != i->end(); j++)
				if (_trajectories_board[*j] == 1)
					c++;
			// Если точек, входящих только в эту траекторию, > 1, то исключаем эту траекторию.
			if (c > 1)
			{
				need_exclude = true;
				i->exclude();
				unproject(*i);
			}
		}

		return need_exclude;
	}
	void exclude_unnecessary_trajectories()
	{
		while (exclude_unnecessary_trajectories(player_red) || exclude_unnecessary_trajectories(player_black));
	}
	// Исключает составные траектории.
	inline void exclude_composite_trajectories(player cur_player)
	{
		list<trajectory>::iterator i, j, k;

		for (k = _trajectories[cur_player].begin(); k != _trajectories[cur_player].end(); k++)
			for (i = _trajectories[cur_player].begin(); i != --_trajectories[cur_player].end(); i++)
				if (k->size() > i->size())
					for (j = i, j++; j != _trajectories[cur_player].end(); j++)
						if (k->size() > j->size() && k->hash() == get_intersect_hash(*i, *j))
							k->exclude();
	}
	inline void exclude_composite_trajectories()
	{
		exclude_composite_trajectories(player_red);
		exclude_composite_trajectories(player_black);
	}
	template<typename _Cont>
	inline void get_points(_Cont &moves, player cur_player)
	{
		for (auto i = _trajectories[cur_player].begin(); i != _trajectories[cur_player].end(); i++)
			if (!i->excluded())
				for (auto j = i->begin(); j != i->end(); j++)
					if (find(moves.begin(), moves.end(), *j) == moves.end())
						moves.push_back(*j);
	}

public:
	trajectories(field &cur_field, zobrist &cur_zobrist)
	{
		_field = &cur_field;
		_depth[player_red] = 0;
		_depth[player_black] = 0;
		_trajectories_board = new int[cur_field.length()];
		fill_n(_trajectories_board, _field->length(), 0);
		_zobrist = &cur_zobrist;
	}
	trajectories(field &cur_field, zobrist &cur_zobrist, size_t depth)
	{
		_field = &cur_field;
		_depth[get_cur_player()] = (depth + 1) / 2;
		_depth[get_enemy_player()] = depth / 2;
		_trajectories_board = new int[cur_field.length()];
		fill_n(_trajectories_board, _field->length(), 0);
		_zobrist = &cur_zobrist;
	}
	trajectories(const trajectories &other)
	{
		_field = other._field;
		_depth[player_red] = other._depth[player_red];
		_depth[player_black] = other._depth[player_black];
		_trajectories_board = new int[_field->length()];
		fill_n(_trajectories_board, _field->length(), 0);
		_zobrist = other._zobrist;
	}
	~trajectories()
	{
		delete _trajectories_board;
	}
	inline player get_cur_player()
	{
		return _field->get_player();
	}
	inline player get_enemy_player()
	{
		return next_player(_field->get_player());
	}
	void clear()
	{
		_trajectories[player_red].clear();
		_trajectories[player_black].clear();
	}
	void build_trajectories()
	{
		if (_depth[get_cur_player()] > 0)
			build_trajectories_recursive(_depth[get_cur_player()] - 1, get_cur_player());
		if (_depth[get_enemy_player()] > 0)
			build_trajectories_recursive(_depth[get_enemy_player()] - 1, get_enemy_player());
	}
	void build_trajectories(trajectories &last, pos cur_pos)
	{
		_field = last._field;
		_depth[get_cur_player()] = last._depth[get_cur_player()];
		_depth[get_enemy_player()] = last._depth[get_enemy_player()] - 1;

		if (_depth[get_cur_player()] > 0)
			build_trajectories_recursive(_depth[get_cur_player()] - 1, get_cur_player());

		if (_depth[get_enemy_player()] > 0)
			for (auto i = last._trajectories[get_enemy_player()].begin(); i != last._trajectories[get_enemy_player()].end(); i++)
				if ((i->size() <= _depth[get_enemy_player()] || (i->size() == _depth[get_enemy_player()] + 1 && find(i->begin(), i->end(), cur_pos) != i->end())) && i->is_valid(*_field, cur_pos))
					add_trajectory(*i, cur_pos, get_enemy_player());
	}
	template<typename _Cont>
	void get_points(_Cont &moves)
	{
		exclude_composite_trajectories();
		// Проецируем неисключенные траектории на доску.
		project();
		// Исключаем те траектории, у которых имеется более одной точки, принадлежащей только ей.
		exclude_unnecessary_trajectories();
		// Получаем список точек, входящих в оставшиеся неисключенные траектории.
		moves.clear();
		get_points(moves, player_red);
		get_points(moves, player_black);
#if ALPHABETA_SORT
		// Сортируем точки по убыванию количества траекторий, в которые они входят.
		sort(moves.begin(), moves.end(), [&](uint x, uint y){ return _trajectories_board[x] < _trajectories_board[y]; });
#endif
		// Очищаем доску от проекций.
		unproject();
		// После получения списка ходов обратно включаем в рассмотрение все траектории (для следующего уровня рекурсии).
		include_all_trajectories();
	}
};