#pragma once
#include <limits.h>

// ��������� 3 ����������� �������� ������� � ������� �� ����������� ����������� � ���������� ����������.
// ����������� ���� p_int.
typedef long p_int;
// ������ ���� p_int.
#define SIZEOF_P_INT 4
// �������� �������������.
#define INFINITY LONG_MAX

// ��������� ��������� �����.
struct Point
{
	p_int X, Y;
};

typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;