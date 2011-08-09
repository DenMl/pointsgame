#pragma once

#include "Config.h"
#include "BasicTypes.h"
#include "Field.h"
#include "GameStack.h"

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

double UCTEstimate(Field &MainField, ulong MaxSimulations, GameStack<uint, MAX_CHAIN_POINTS> &Moves);
double UCTEstimateWithTime(Field &MainField, ulong Time, GameStack<uint, MAX_CHAIN_POINTS> &Moves);