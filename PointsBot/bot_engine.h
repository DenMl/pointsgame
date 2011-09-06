#pragma once

#include "config.h"
#include "basic_types.h"
#include "field.h"

using namespace std;

// ���������� ������ ��������� ���.
// ���� �� ������� - -1.
pos position_estimate_best_move(field &cur_field);
pos minmax_best_move(field &cur_field, size_t depth);
pos uct_best_move(field &cur_field, size_t max_simulations);
pos uct_with_time_best_move(field &cur_field, size_t time);
pos minmax_position_estimate_best_move(field &cur_field, size_t depth);
pos minmax_uct_best_move(field &cur_field, size_t depth, size_t max_simulations);
pos minmax_uct_with_time_best_move(field &cur_field, size_t depth, size_t time);
pos minmax_uct_with_time_position_estimate_best_move(field &cur_field, size_t depth, size_t time);