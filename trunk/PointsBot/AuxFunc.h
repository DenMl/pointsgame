#pragma once
#include "BasicTypes.h"

inline _int Abs(_int x)
{
	return x >= 0 ? x : -x;
}

inline void Swap(_int &a, _int &b)
{
	_int c = a;
	a = b;
	b = c;
}