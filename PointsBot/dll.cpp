#include "config.h"
#include "basic_types.h"
#include "dll.h"
#include "field.h"
#include "bot.h"
#include <Windows.h>

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

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

bot* init(coord width, coord height, ptrdiff_t seed)
{
	return new bot(width, height, SUR_COND_STANDART, BEGIN_PATTERN_CLEAN, seed);
}

void final(bot* cur_bot)
{
	delete cur_bot;
}

void put_point(bot* cur_bot, coord x, coord y, player cur_player)
{
	cur_bot->do_step(x, y, cur_player);
}

void remove_last_point(bot* cur_bot)
{
	cur_bot->undo_step();
}

void get_move(bot* cur_bot, coord& x, coord& y, player cur_player)
{
	cur_bot->set_player(cur_player);
	cur_bot->minimax_uct_best_move(x, y, 8, 100000);
}

void get_move_with_complexity(field* cur_field, coord& x, coord& y, player cur_player, int complexity)
{

}

void get_move_with_time(field* cur_field, coord& x, coord& y, player cur_player, int time)
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