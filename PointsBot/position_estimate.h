#pragma once

#include "config.h"
#include "basic_types.h"
#include "field.h"

using namespace std;

const short cg_summa[] = {-5, -1, 0, 0, 1, 2, 5, 20, 30};

short position_estimate(field& cur_field, uint cur_pos, short cur_player);
pos position_estimate(field& cur_field, list<pos>& moves);