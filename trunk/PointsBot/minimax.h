#pragma once

#include "config.h"
#include "basic_types.h"
#include "field.h"
#include "trajectories.h"

using namespace std;

score alphabeta(field* cur_field, size_t depth, pos cur_pos, trajectories* last, score alpha, score beta, int* empty_board);
pos minimax(field* cur_field, size_t depth);