#pragma once

#include "config.h"
#include "basic_types.h"
#include "field.h"

using namespace std;

const short cg_summa[] = {-5, -1, 0, 0, 1, 2, 5, 20, 30};

class position_estimate
{
private:
	field* _field;

	short get_estimate(uint cur_pos, short cur_player);

public:
	position_estimate(field* cur_field);
	pos get(list<pos> &moves);
};