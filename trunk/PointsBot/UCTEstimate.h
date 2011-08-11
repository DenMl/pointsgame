#pragma once

#include "Config.h"
#include "BasicTypes.h"
#include "Field.h"
#include "static_vector.h"

const double UCTK = 1;
// ������, ������ �������� ���������� ������.
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

double UCTEstimate(Field &MainField, ulong MaxSimulations, static_vector<uint, MAX_CHAIN_POINTS> &Moves);
double UCTEstimateWithTime(Field &MainField, ulong Time, static_vector<uint, MAX_CHAIN_POINTS> &Moves);