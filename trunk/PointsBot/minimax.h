#pragma once

#include "config.h"
#include "basic_types.h"
#include "field.h"
#include "trajectories.h"

using namespace std;

class minimax
{
private:
	field* _field;

	score negamax(field &cur_field, uint depth, uint cur_pos, trajectories &last, int alpha, int beta);
public:
	minimax(field* cur_field);
	pos get(size_t depth, list<pos> &moves);
};