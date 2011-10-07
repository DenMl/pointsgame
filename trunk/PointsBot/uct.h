#pragma once

#include "config.h"
#include "basic_types.h"
#include "field.h"
#include <list>

using namespace std;

struct uct_node
{
	ulong wins;
	ulong visits;
	uint move;
	uct_node* child;
	uct_node* sibling;

	uct_node()
	{
		wins = 0;
		visits = 0;
		move = 0;
		child = NULL;
		sibling = NULL;
	}
};

pos uct(field &cur_field, size_t max_simulations, list<pos> &moves);
pos uct_with_time(field &cur_field, size_t time, list<pos> &moves);