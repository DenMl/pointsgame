#pragma once
#include "BasicTypes.h"
#include "Field.h"

const p_int UCTK = 1;

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

struct Rect
{
	p_int Left, Right, Top, Bottom;
};

void UCTEstimate(Field &MainField, p_int MaxSimulations, GameStack<p_int, MAX_CHAIN_POINTS> &Moves);