#include "Config.h"
#include "Field.h"

using namespace std;

short field::get_input_points(const pos CenterPos, const value EnableCond, pos InpChainPoints[], pos InpSurPoints[]) const
{
	short result = 0;

	if (is_not_enable(w(CenterPos), EnableCond))
	{
		if (is_enable(nw(CenterPos), EnableCond))
		{
			InpChainPoints[0] = nw(CenterPos);
			InpSurPoints[0] = w(CenterPos);
			result++;
		}
		else if (is_enable(n(CenterPos), EnableCond))
		{
			InpChainPoints[0] = n(CenterPos);
			InpSurPoints[0] = w(CenterPos);
			result++;
		}
	}

	if (is_not_enable(s(CenterPos), EnableCond))
	{
		if (is_enable(sw(CenterPos), EnableCond))
		{
			InpChainPoints[result] = sw(CenterPos);
			InpSurPoints[result] = s(CenterPos);
			result++;
		}
		else if (is_enable(w(CenterPos), EnableCond))
		{
			InpChainPoints[result] = w(CenterPos);
			InpSurPoints[result] = s(CenterPos);
			result++;
		}
	}

	if (is_not_enable(e(CenterPos), EnableCond))
	{
		if (is_enable(se(CenterPos), EnableCond))
		{
			InpChainPoints[result] = se(CenterPos);
			InpSurPoints[result] = e(CenterPos);
			result++;
		}
		else if (is_enable(s(CenterPos), EnableCond)) 
		{
			InpChainPoints[result] = s(CenterPos);
			InpSurPoints[result] = e(CenterPos);
			result++;
		}
	}

	if (is_not_enable(n(CenterPos), EnableCond))
	{
		if (is_enable(ne(CenterPos), EnableCond))
		{
			InpChainPoints[result] = ne(CenterPos);
			InpSurPoints[result] = n(CenterPos);
			result++;
		}
		else if (is_enable(e(CenterPos), EnableCond))
		{
			InpChainPoints[result] = e(CenterPos);
			InpSurPoints[result] = n(CenterPos);
			result++;
		}
	}

	return result;
}

void field::place_begin_pattern(begin_pattern cur_pattern)
{
	switch (cur_pattern)
	{
	case (BP_CROSSWIRE):
		do_step(to_pos(_width / 2 - 1, _height / 2 - 1));
		do_step(to_pos(_width / 2, _height / 2 - 1));
		do_step(to_pos(_width / 2, _height / 2));
		do_step(to_pos(_width / 2 - 1, _height / 2));
		break;
	case (BP_SQUARE):
		do_step(to_pos(_width / 2 - 1, _height / 2 - 1));
		do_step(to_pos(_width / 2, _height / 2 - 1));
		do_step(to_pos(_width / 2 - 1, _height / 2));
		do_step(to_pos(_width / 2, _height / 2));
		break;
	}
}

void field::wave(pos start_pos, function<bool(pos)> cond)
{
	// Очередь для волнового алгоритма (обхода в ширину).
	list<pos> q;

	q.push_back(start_pos);
	set_tag(start_pos);
	auto it = q.begin();
	while (it != q.end())
	{
		if (!is_tagged(w(*it)) && cond(w(*it)))
		{
			q.push_back(w(*it));
			set_tag(w(*it));
		}
		if (!is_tagged(n(*it)) && cond(n(*it)))
		{
			q.push_back(n(*it));
			set_tag(n(*it));
		}
		if (!is_tagged(e(*it)) && cond(e(*it)))
		{
			q.push_back(e(*it));
			set_tag(e(*it));
		}
		if (!is_tagged(s(*it)) && cond(s(*it)))
		{
			q.push_back(s(*it));
			set_tag(s(*it));
		}
		it++;
	}

	for (it = q.begin(); it != q.end(); it++)
		clear_tag(*it);
}

void field::remove_empty_base(const pos start_pos)
{
	wave(	start_pos,
			[&](pos Pos)->bool
			{
				if (is_in_empty_base(Pos))
				{
					_changes.back().changes.push(pair<pos, value>(Pos, _points[Pos] & !tag_bit));
					clear_empty_base(Pos);
					return true;
				}
				else
				{
					return false;
				}
			});
}

bool field::build_chain(const pos start_pos, const value enable_cond, const pos direction_pos, list<pos> &chain)
{
	chain.clear();
	chain.push_back(start_pos);
	pos Pos = direction_pos;
	pos CenterPos = start_pos;
	// Площадь базы.
	int TempSquare = square(CenterPos, Pos);
	do
	{
		if (is_tagged(Pos))
		{
			while (chain.back() != Pos)
			{
				clear_tag(chain.back());
				chain.pop_back();
			}
		}
		else
		{
			set_tag(Pos);
			chain.push_back(Pos);
		}
		swap(Pos, CenterPos);
		get_first_next_pos(CenterPos, Pos);
		while (is_not_enable(Pos, enable_cond))
			get_next_pos(CenterPos, Pos);
		TempSquare += square(CenterPos, Pos);
	}
	while (Pos != start_pos);

	for (auto i = chain.begin(); i != chain.end(); i++)
		clear_tag(*i);

	return (TempSquare < 0 && chain.size() > 2);
}

void field::find_surround(list<pos> &chain, pos inside_point, player cur_player)
{
	// Количество захваченных точек.
	int CurCaptureCount = 0;
	// Количество захваченных пустых полей.
	int CurFreedCount = 0;

	list<pos> SurPoints;

	// Помечаем точки цепочки.
	for (auto i = chain.begin(); i != chain.end(); i++)
		set_tag(*i);

	wave(	inside_point,
			[&, cur_player](pos Pos)->bool
			{
				if (is_not_bound(Pos, cur_player | put_bit | bound_bit))
				{
					check_captured_and_freed(Pos, cur_player, CurCaptureCount, CurFreedCount);
					SurPoints.push_back(Pos);
					return true;
				}
				else
				{
					return false;
				}
	});
	// Изменение счета игроков.
	add_sub_captured_freed(cur_player, CurCaptureCount, CurFreedCount);

#if SURROUND_CONDITIONS
	if ((CurCaptureCount != 0) || (_sur_cond == SC_ALWAYS)) // Если захватили точки, или стоит опция захватывать всегда.
#else
	if (CurCaptureCount != 0) // Если захватили точки.
#endif
	{
		for (auto i = chain.begin(); i != chain.end(); i++)
		{
			clear_tag(*i);
			// Добавляем в список изменений точки цепочки.
			_changes.back().changes.push(pair<pos, value>(*i, _points[*i]));
			// Помечаем точки цепочки.
			set_base_bound(*i);
		}

		for (auto i = SurPoints.begin(); i != SurPoints.end(); i++)
		{
			_changes.back().changes.push(pair<pos, value>(*i, _points[*i]));

			set_capture_free_state(*i, cur_player);
		}
	}
	else // Если ничего не захватили.
	{
		for (auto i = chain.begin(); i != chain.end(); i++)
			clear_tag(*i);

		for (auto i = SurPoints.begin(); i != SurPoints.end(); i++)
		{
			_changes.back().changes.push(pair<pos, value>(*i, _points[*i]));

			if (!is_putted(*i))
				set_empty_base(*i);
		}
	}
}

field::field(const coord width, const coord height, const sur_cond sur_cond, const begin_pattern begin_pattern)
{
	_width = width;
	_height = height;
#if SURROUND_CONDITIONS
	_sur_cond = sur_cond;
#endif
	_player = player_red;
	_capture_count[player_red] = 0;
	_capture_count[player_black] = 0;

	_points = new value[length()];
	fill_n(_points, length(), 0);

	for (coord x = -1; x <= width; x++)
	{
		set_bad(to_pos(x, -1));
		set_bad(to_pos(x, height));
	}
	for (coord y = -1; y <= height; y++)
	{
		set_bad(to_pos(-1, y));
		set_bad(to_pos(width, y));
	}

	_changes.reserve(length());
	points_seq.reserve(length());

	_hash = 0;

	place_begin_pattern(begin_pattern);
}

field::field(const field &orig)
{
	_width = orig._width;
	_height = orig._height;
#if SURROUND_CONDITIONS
	_sur_cond = orig._sur_cond;
#endif
	_player = orig._player;
	_capture_count[player_red] = orig._capture_count[player_red];
	_capture_count[player_black] = orig._capture_count[player_black];

	_points = new value[length()];
	copy_n(orig._points, length(), _points);

	_changes.reserve(length());
	points_seq.reserve(length());

	_changes.assign(orig._changes.begin(), orig._changes.end());
	points_seq.assign(orig.points_seq.begin(), orig.points_seq.end());

	_hash = orig._hash;
}

field::~field()
{
	delete _points;
}

bool field::is_point_inside_ring(const pos cur_pos, const list<pos> &ring) const
{
	uint intersections = 0;

	intersection_state state = IS_NONE;

	for (auto i = ring.begin(); i != ring.end(); i++)
	{
		switch (get_intersection_state(cur_pos, *i))
		{
		case (IS_NONE):
			state = IS_NONE;
			break;
		case (IS_UP):
			if (state == IS_DOWN)
				intersections++;
			state = IS_UP;
			break;
		case (IS_DOWN):
			if (state == IS_UP)
				intersections++;
			state = IS_DOWN;
			break;
		}
	}
	if (state == IS_UP || state == IS_DOWN)
	{
		auto i = ring.begin();
		intersection_state TempState = get_intersection_state(cur_pos, *i);
		while (TempState == state || TempState == IS_TARGET)
		{
			i++;
			TempState = get_intersection_state(cur_pos, *i);
		}
		if (TempState != IS_NONE)
			intersections++;
	}

	return intersections % 2 == 1;
}

void field::check_closure(const pos start_pos, player cur_player)
{
	short inp_points_count;
	pos inp_chain_points[4], inp_sur_points[4];

	list<pos> chain;

	if (is_in_empty_base(start_pos)) // Если точка поставлена в пустую базу.
	{
		// Проверяем, в чью пустую базу поставлена точка.
		pos Pos = start_pos - 1;
		while (!is_putted(Pos))
			Pos--;

		if (get_player(Pos) == get_player(start_pos)) // Если поставили в свою пустую базу.
		{
			clear_empty_base(start_pos);
			return;
		}

#if SURROUND_CONDITIONS
		if (_sur_cond != SC_ALWAYS_ENEMY) // Если приоритет не всегда у врага.
#endif
		{
			inp_points_count = get_input_points(start_pos, cur_player | put_bit, inp_chain_points, inp_sur_points);
			if (inp_points_count > 1)
			{
				short ChainsCount = 0;
				for (short i = 0; i < inp_points_count; i++)
					if (build_chain(start_pos, get_player(start_pos) | put_bit, inp_chain_points[i], chain))
					{
						find_surround(chain, inp_sur_points[i], cur_player);
						ChainsCount++;
						if (ChainsCount == inp_points_count - 1)
							break;
					}
					if (is_base_bound(start_pos))
					{
						remove_empty_base(start_pos);
						return;
					}
			}
		}

		Pos++;
		do
		{
			Pos--;
			while (!is_enable(Pos, next_player(cur_player) | put_bit))
				Pos--;
			inp_points_count = get_input_points(Pos, next_player(cur_player) | put_bit, inp_chain_points, inp_sur_points);
			for (short i = 0; i < inp_points_count; i++)
				if (build_chain(Pos, next_player(cur_player) | put_bit, inp_chain_points[i], chain))
					if (is_point_inside_ring(start_pos, chain))
					{
						find_surround(chain, inp_sur_points[i], get_player(Pos));
						break;
					}
		} while (!is_captured(start_pos));
	}
	else
	{
		inp_points_count = get_input_points(start_pos, cur_player | put_bit, inp_chain_points, inp_sur_points);
		if (inp_points_count > 1)
		{
			short ChainsCount = 0;
			for (short i = 0; i < inp_points_count; i++)
				if (build_chain(start_pos, get_player(start_pos) | put_bit, inp_chain_points[i], chain))
				{
					find_surround(chain, inp_sur_points[i], cur_player);
					ChainsCount++;
					if (ChainsCount == inp_points_count - 1)
						break;
				}
		}
	}
}

bool field::check_closure(const pos start_pos, const pos checked_pos, player cur_player)
{
	pos InpChainPoints[4], InpSurPoints[4];

	list<pos> chain;

	short InpPointsCount = get_input_points(start_pos, cur_player | put_bit, InpChainPoints, InpSurPoints);
	if (InpPointsCount > 1)
	{
		for (ushort i = 0; i < InpPointsCount; i++)
			if (build_chain(start_pos, get_player(start_pos) | put_bit, InpChainPoints[i], chain))
				if (is_point_inside_ring(checked_pos, chain))
				{
					for (auto j = chain.begin(); j != chain.end(); j++)
					{
						// Добавляем в список изменений точки цепочки.
						_changes.back().changes.push(pair<pos, value>(*j, _points[*j]));
						// Помечаем точки цепочки.
						set_base_bound(*j);
					}
					return true;
				}
	}

	return false;
}