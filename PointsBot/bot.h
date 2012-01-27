#pragma once

#include "config.h"
#include "basic_types.h"
#include "player.h"
#include "field.h"
#include "position_estimate.h"
#include "uct.h"
#include "minimax.h"
#include "zobrist.h"

using namespace std;

class bot
{
private:
	mt* _gen;
	zobrist* _zobrist;
	field* _field;
	void build_all_moves(list<pos>& moves) const;
	size_t get_minimax_depth(size_t complexity);
	size_t get_uct_iterations(size_t complexity);
public:
	bot(const coord width, const coord height, const begin_pattern begin_pattern, ptrdiff_t seed);
	~bot();
	bool do_step(coord x, coord y, player cur_player);
	bool undo_step();
	void set_player(player cur_player);
	bool boundary_check(coord& x, coord& y);
	// Возвращает лучший найденный ход.
	void get(coord& x, coord& y);
	void get_with_complexity(coord& x, coord& y, size_t complexity);
	void get_with_time(coord& x, coord& y, size_t time);
};