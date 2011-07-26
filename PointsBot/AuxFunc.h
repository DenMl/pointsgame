#pragma once
#include "BasicTypes.h"

// Возвращает модуль числа x.
inline p_int Abs(p_int x)
{
	return x >= 0 ? x : -x;
}

// Меняет местами значения переменных a и b.
inline void Swap(p_int &a, p_int &b)
{
	p_int c = a;
	a = b;
	b = c;
}