#pragma once

#include "Config.h"
#include "basic_types.h"
#include "Field.h"
#include <list>

using namespace std;

#define UCTK 1
// Радиус, внутри которого происходит анализ UCT.
#define UCT_RADIUS 2
#define UCT_ITERATIONS_BEFORE_CHECK_TIME 100

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