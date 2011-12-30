#pragma once

#include "config.h"
#include "basic_types.h"
#include "player.h"
#include "field.h"
#include "position_estimate.h"

using namespace std;

class bot
{
private:
	field* _field;
	position_estimate* _position_estimate;
	void build_all_moves(list<pos>& moves) const;
public:
	bot(const coord width, const coord height, const sur_cond sur_cond, const begin_pattern begin_pattern);
	~bot();
	void do_step(coord x, coord y, player cur_player);
	void undo_step();
	void set_player(player cur_player);
	// Возвращает лучший найденный ход.
	// Если не найдено - -1.
	void position_estimate_best_move(coord& x, coord& y);
	void minimax_best_move(coord& x, coord& y, size_t depth);
	void uct_best_move(coord& x, coord& y, size_t max_simulations);
	void uct_with_time_best_move(coord& x, coord& y, size_t time);
	void minimax_uct_best_move(coord& x, coord& y, size_t depth, size_t max_simulations);
	void minimax_uct_with_time_best_move(coord& x, coord& y, size_t depth, size_t time);
};