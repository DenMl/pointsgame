#pragma once

#include "Config.h"
#include <list>

using namespace std;

// ���������� ������ ����� x.
template<typename TYPE> inline const TYPE Abs(TYPE x)
{
	return x >= 0 ? x : -x;
}

// ������ ������� �������� ���������� a � b.
template<typename TYPE> inline const void Swap(TYPE &a, TYPE &b)
{
	TYPE c = a;
	a = b;
	b = c;
}

// � PotentialMoves ���� ����������� PotentialMoves � Moves.
//PotentialMoves.remove_if([&](uint value){return find(Moves.begin(), Moves.end(), value) == Moves.end();});