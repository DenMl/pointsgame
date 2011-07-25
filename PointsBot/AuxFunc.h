#pragma once
#include "BasicTypes.h"

inline p_int Abs(p_int x)
{
	return x >= 0 ? x : -x;
}

inline void Swap(p_int &a, p_int &b)
{
	p_int c = a;
	a = b;
	b = c;
}