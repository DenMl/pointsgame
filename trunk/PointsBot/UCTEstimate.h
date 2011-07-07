#pragma once
#include "BasicTypes.h"
#include "Field.h"

const _int UCTK = 1;

struct Node
{
	_int Wins;
	_int Visits;
	_int Move;
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
	_int Left, Right, Top, Bottom;
};

void UCTEstimate(Field &MainField, _int MaxSimulations, GameStack<_int, MAX_CHAIN_POINTS> &Moves);