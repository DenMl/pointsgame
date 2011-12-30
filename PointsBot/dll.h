#pragma once

#include "config.h"
#include "bot.h"

#define DLLEXPORT extern "C" __declspec(dllexport)

DLLEXPORT bot* init(coord width, coord height);
DLLEXPORT void final(bot* cur_bot);
DLLEXPORT void put_point(bot* cur_bot, coord x, coord y, player cur_player);
DLLEXPORT void remove_last_point(bot* cur_bot);
DLLEXPORT void get_move(bot* cur_bot, coord& x, coord& y, player cur_player);
DLLEXPORT void get_move_with_complexity(bot* cur_bot, coord& x, coord& y, player cur_player, int complexity);
DLLEXPORT void get_move_with_time(bot* cur_bot, coord& x, coord& y, player cur_player, int time);
DLLEXPORT wchar_t* get_name();
DLLEXPORT wchar_t* get_version();