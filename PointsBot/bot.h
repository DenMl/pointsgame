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
	uct* _uct;
	minimax* _minimax;
	void build_all_moves(list<pos>& moves) const;
public:
	bot(const coord width, const coord height, const begin_pattern begin_pattern, ptrdiff_t seed);
	~bot();
	bool do_step(coord x, coord y, player cur_player);
	bool undo_step();
	void set_player(player cur_player);
	bool boundary_check(coord& x, coord& y);
	// Возвращает лучший найденный ход.
	// Если не найдено - -1.
	void position_estimate_best_move(coord& x, coord& y);
	void minimax_best_move(coord& x, coord& y, size_t depth);
	void uct_best_move(coord& x, coord& y, size_t max_simulations);
	void uct_with_time_best_move(coord& x, coord& y, size_t time);
	void minimax_uct_best_move(coord& x, coord& y, size_t depth, size_t max_simulations);
	void minimax_uct_with_time_best_move(coord& x, coord& y, size_t depth, size_t time);
};