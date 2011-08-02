#pragma once
#include <limits.h>

// Следующие 3 определения задаются вручную и зависят от конкретного компилятора и параметров компиляции.
// Определение типа p_int.
typedef long p_int;
// Размер типа p_int.
#define SIZEOF_P_INT 4
// Значение бесконечности.
#define INFINITY LONG_MAX

// Структура координат точки.
struct Point
{
	p_int X, Y;
};

typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;