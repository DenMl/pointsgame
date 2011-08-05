#pragma once
#include "BasicTypes.h"
#include "Field.h"

const p_int UCTK = 1;
// Радиус, внутри которого происходит анализ.
const p_int UCTRadius = 2;
const int IterationsBeforeCheckTime = 100;

struct Node
{
	p_int Wins;
	p_int Visits;
	p_int Move;
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

float UCTEstimate(Field &MainField, p_int MaxSimulations, GameStack<p_int, MAX_CHAIN_POINTS> &Moves);
float UCTEstimateWithTime(Field &MainField, p_int Time, GameStack<p_int, MAX_CHAIN_POINTS> &Moves);