#include "config.h"
#include "basic_types.h"
#include "points_bot.h"
#include "field.h"
#include "bot_engine.h"
#include "Random.h"

const uint min_minimax_depth = 0;
const uint max_minimax_depth = 8;
const ulong min_uct_iterations = 0;
const ulong max_uct_iterations = 100000;
const ulong min_p = 0;
const ulong max_p = 100;

inline uint get_minimax_depth(ulong p)
{
	return (p - min_p) * (max_minimax_depth - min_minimax_depth) / (max_p - min_p) + min_minimax_depth;
}

inline ulong get_uct_iterations(ulong p)
{
	return (p - min_p) * (max_uct_iterations - min_uct_iterations) / (max_p - min_p) + min_uct_iterations;
}

field* init(int width, int height)
{
	return new field(static_cast<coord>(width), static_cast<coord>(height), SUR_COND_STANDART, BEGIN_PATTERN_CLEAN);
}

void final(field* cur_field)
{
	delete cur_field;
}

void put_point(field* cur_field, int x, int y, int cur_player)
{
	cur_field->do_step(cur_field->to_pos(static_cast<coord>(x), static_cast<coord>(y)), cur_player);
}

void remove_last_point(field* cur_field)
{
	cur_field->undo_step();
}

void get_move(field* cur_field, int& x, int& y, int cur_player)
{
	pos cur_pos;
	cur_field->set_player(static_cast<player>(cur_player));
	cur_pos = minimax_uct_best_move(*cur_field, 8, 100000);
	x = cur_field->to_x(cur_pos);
	y = cur_field->to_y(cur_pos);
}

void get_move_with_complexity(field* cur_field, int& x, int& y, int cur_player, int complexity)
{

}

void get_move_with_time(field* cur_field, int& x, int& y, int cur_player, int time)
{

}

wchar_t* get_name()
{
	return L"kkai";
}

wchar_t* get_version()
{
	return L"2.0.0.0";
}