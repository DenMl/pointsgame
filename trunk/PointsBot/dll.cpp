#include "config.h"
#include "basic_types.h"
#include "dll.h"
#include "field.h"
#include "bot.h"
#include <Windows.h>

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
	return new bot(width, height, BEGIN_PATTERN_CLEAN, seed);
}

void final(bot* cur_bot)
{
	delete cur_bot;
}

void play(bot* cur_bot, coord x, coord y, player cur_player)
{
	cur_bot->do_step(x, y, cur_player);
}

void undo(bot* cur_bot)
{
	cur_bot->undo_step();
}

void gen_move(bot* cur_bot, coord& x, coord& y, player cur_player)
{
	cur_bot->set_player(cur_player);
	cur_bot->get(x, y);
}

void gen_move_with_complexity(bot* cur_bot, coord& x, coord& y, player cur_player, int complexity)
{
	cur_bot->set_player(cur_player);
	cur_bot->get_with_complexity(x, y, complexity);
}

void gen_move_with_time(bot* cur_bot, coord& x, coord& y, player cur_player, int time)
{
	cur_bot->set_player(cur_player);
	cur_bot->get_with_time(x, y, time);
}

wchar_t* name()
{
	return L"kkai";
}

wchar_t* version()
{
	return L"2.0.0.0";
}