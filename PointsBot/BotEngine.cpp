#include "config.h"
#include "basic_types.h"
#include "BotEngine.h"
#include "Field.h"
#include "MinMaxEstimate.h"
#include "uct.h"
#include "position_estimate.h"
#include "Random.h"
#include <list>

using namespace std;

void build_all_moves(field &cur_field, list<pos> &moves)
{
	moves.clear();
	for (pos i = cur_field.min_pos(); i <= cur_field.max_pos(); i++)
		if (cur_field.putting_allow(i))
			moves.push_back(i);
}

pos position_estimate_best_move(field &cur_field)
{
	list<pos> moves;
	build_all_moves(cur_field, moves);
	return position_estimate(cur_field, moves);
}

pos uct_best_move(field &cur_field, size_t max_simulations)
{
	list<pos> moves;
	build_all_moves(cur_field, moves);
	return uct(cur_field, max_simulations, moves);
}

pos uct_with_time_best_move(field &cur_field, size_t time)
{
	list<pos> moves;
	build_all_moves(cur_field, moves);
	return uct_with_time(cur_field, time, moves);
}