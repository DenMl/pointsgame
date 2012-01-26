#include "config.h"
#include "basic_types.h"
#include "bot.h"
#include "time.h"
#include "field.h"
#include "minimax.h"
#include "uct.h"
#include "position_estimate.h"
#include "zobrist.h"
#include <list>

using namespace std;

bot::bot(const coord width, const coord height, const begin_pattern begin_pattern, ptrdiff_t seed)
{
	_gen = new mt(seed);
	_zobrist = new zobrist((width + 2) * (height + 2), _gen);
	_field = new field(width, height, begin_pattern, _zobrist);
}

bot::~bot()
{
	delete _field;
	delete _zobrist;
	delete _gen;
}

void bot::build_all_moves(list<pos> &moves) const
{
	moves.clear();
	for (pos i = _field->min_pos(); i <= _field->max_pos(); i++)
		if (_field->putting_allow(i))
			moves.push_back(i);
}

bool bot::do_step(coord x, coord y, player cur_player)
{
	return _field->do_step(_field->to_pos(static_cast<coord>(x), static_cast<coord>(y)), cur_player);
}

bool bot::undo_step()
{
	if (_field->points_seq.size() == 0)
		return false;
	_field->undo_step();
	return true;
}

void bot::set_player(player cur_player)
{
	_field->set_player(cur_player);
}

bool bot::boundary_check(coord& x, coord& y)
{
	if (_field->points_seq.size() == 0)
	{
		x = _field->width() / 2;
		y = _field->height() / 2;
		return true;
	}
	return false;
}

void bot::position_estimate_best_move(coord& x, coord& y)
{
	list<pos> moves;
	if (boundary_check(x, y))
		return;
	build_all_moves(moves);
	if (moves.size() == 0)
	{
		x = -1;
		y = -1;
		return;
	}
	pos result = position_estimate(*_field, moves);
	x = _field->to_x(result);
	y = _field->to_y(result);
}

void bot::minimax_best_move(coord& x, coord& y, size_t depth)
{
	list<pos> moves;
	if (boundary_check(x, y))
		return;
	build_all_moves(moves);
	if (moves.size() == 0)
	{
		x = -1;
		y = -1;
		return;
	}
	pos result =  minimax(*_field, depth, moves);
	if (result == -1)
		result = position_estimate(*_field, moves);
	x = _field->to_x(result);
	y = _field->to_y(result);
}

void bot::uct_best_move(coord& x, coord& y, size_t max_simulations)
{
	list<pos> moves;
	if (boundary_check(x, y))
		return;
	build_all_moves(moves);
	if (moves.size() == 0)
	{
		x = -1;
		y = -1;
		return;
	}
	pos result = uct(*_field, *_gen, max_simulations, moves);
	x = _field->to_x(result);
	y = _field->to_y(result);
}

void bot::uct_with_time_best_move(coord& x, coord& y, size_t time)
{
	list<pos> moves;
	if (boundary_check(x, y))
		return;
	build_all_moves(moves);
	if (moves.size() == 0)
	{
		x = -1;
		y = -1;
		return;
	}
	pos result = uct_with_time(*_field, *_gen, time, moves);
	x = _field->to_x(result);
	y = _field->to_y(result);
}

void bot::minimax_uct_best_move(coord& x, coord& y, size_t depth, size_t max_simulations)
{
	list<pos> moves;
	if (boundary_check(x, y))
		return;
	build_all_moves(moves);
	if (moves.size() == 0)
	{
		x = -1;
		y = -1;
		return;
	}
	pos result =  minimax(*_field, depth, moves);
	if (result == -1)
		result = uct(*_field, *_gen, max_simulations, moves);
	x = _field->to_x(result);
	y = _field->to_y(result);
}

void bot::minimax_uct_with_time_best_move(coord& x, coord& y, size_t depth, size_t time)
{
	timer t;
	list<pos> moves;
	if (boundary_check(x, y))
		return;
	build_all_moves(moves);
	if (moves.size() == 0)
	{
		x = -1;
		y = -1;
		return;
	}
	pos result =  minimax(*_field, depth, moves);
	ulong minmax_time = t.get();
	if (result == -1 && minmax_time < time)
		result = uct_with_time(*_field, *_gen, time - minmax_time, moves);
	if (result == -1)
		result = position_estimate(*_field, moves);
	x = _field->to_x(result);
	y = _field->to_y(result);
}