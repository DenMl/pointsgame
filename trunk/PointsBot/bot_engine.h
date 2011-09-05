#pragma once

#include "Config.h"
#include "basic_types.h"
#include "Field.h"

using namespace std;

// ���������� ������ ��������� ���.
// ���� �� ������� - -1.
pos position_estimate_best_move(field &cur_field);
pos uct_best_move(field &cur_field, size_t max_simulations);
pos uct_with_time_best_move(field &cur_field, size_t time);