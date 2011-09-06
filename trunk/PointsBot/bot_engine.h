#pragma once

#include "config.h"
#include "basic_types.h"
#include "field.h"

using namespace std;

// Возвращает лучший найденный ход.
// Если не найдено - -1.
pos position_estimate_best_move(field &cur_field);
pos minimax_best_move(field &cur_field, size_t depth);
pos uct_best_move(field &cur_field, size_t max_simulations);
pos uct_with_time_best_move(field &cur_field, size_t time);
pos minimax_position_estimate_best_move(field &cur_field, size_t depth);
pos minimax_uct_best_move(field &cur_field, size_t depth, size_t max_simulations);
pos minimax_uct_with_time_best_move(field &cur_field, size_t depth, size_t time);
pos minimax_uct_with_time_position_estimate_best_move(field &cur_field, size_t depth, size_t time);