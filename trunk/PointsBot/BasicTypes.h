#pragma once
#include <limits.h>

typedef long p_int;

#define SIZEOF_P_INT 4

// Значение бесконечности.
#define INFINITY LONG_MAX

// Структура координат точки.
struct Point
{
	p_int X, Y;
};