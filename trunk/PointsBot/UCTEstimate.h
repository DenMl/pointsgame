#pragma once

#include "Config.h"
#include "BasicTypes.h"
#include "Field.h"
#include "static_vector.h"

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

double UCTEstimate(field &MainField, ulong MaxSimulations, static_vector<pos, MAX_CHAIN_POINTS> &Moves);
double UCTEstimateWithTime(field &MainField, ulong Time, static_vector<pos, MAX_CHAIN_POINTS> &Moves);