#pragma once
#include <limits.h>

typedef long p_int;

#define SIZEOF_P_INT 4

// �������� �������������.
#define INFINITY LONG_MAX

// ��������� ��������� �����.
struct Point
{
	p_int X, Y;
};