#pragma once

#include "Config.h"
#include "BasicTypes.h"
#include "Field.h"
#include <vector>

const double UCTK = 1;
// Радиус, внутри которого происходит анализ.
const ushort UCTRadius = 2;
const uint IterationsBeforeCheckTime = 100;

struct Node
{
	ulong Wins;
	ulong Visits;
	uint Move;
	Node *Child;
	Node *Sibling;

	Node()
	{
		Wins = 0;
		Visits = 0;
		Move = 0;
		Child = NULL;
		Sibling = NULL;
	}
};

double UCTEstimate(Field &MainField, ulong MaxSimulations, vector<uint> &Moves);
double UCTEstimateWithTime(Field &MainField, ulong Time, vector<uint> &Moves);