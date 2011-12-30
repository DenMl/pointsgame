#pragma once

#include "config.h"
#include "field.h"

#define DLLEXPORT extern "C" __declspec(dllexport)

DLLEXPORT field* init(int width, int height);
DLLEXPORT void final(field* cur_field);
DLLEXPORT void put_point(field* cur_field, int x, int y, int cur_player);
DLLEXPORT void remove_last_point(field* cur_field);
DLLEXPORT void get_move(field* cur_field, int& x, int& y, int cur_player);
DLLEXPORT void get_move_with_complexity(field* cur_field, int& x, int& y, int cur_player, int complexity);
DLLEXPORT void get_move_with_time(field* cur_field, int& x, int& y, int cur_player, int time);
DLLEXPORT wchar_t* get_name();
DLLEXPORT wchar_t* get_version();