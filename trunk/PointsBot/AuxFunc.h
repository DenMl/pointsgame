#pragma once

#include "Config.h"

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