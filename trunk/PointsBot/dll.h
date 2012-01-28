#pragma once

#include "config.h"
#include "bot.h"

#define DLLEXPORT extern "C" __declspec(dllexport)

DLLEXPORT bot* init(coord width, coord height, ptrdiff_t seed);
DLLEXPORT void final(bot* cur_bot);
DLLEXPORT void play(bot* cur_bot, coord x, coord y, player cur_player);
DLLEXPORT void undo(bot* cur_bot);
DLLEXPORT void gen_move(bot* cur_bot, coord& x, coord& y, player cur_player);
DLLEXPORT void gen_move_with_complexity(bot* cur_bot, coord& x, coord& y, player cur_player, int complexity);
DLLEXPORT void gen_move_with_time(bot* cur_bot, coord& x, coord& y, player cur_player, int time);
DLLEXPORT wchar_t* name();
DLLEXPORT wchar_t* version();