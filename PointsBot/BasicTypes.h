#pragma once
#include <limits.h>

// ��������� 3 ����������� �������� ������� � ������� �� ����������� ����������� � ���������� ����������.
// ����������� ���� p_int.
typedef int p_int;
// ������ ���� p_int.
#define SIZEOF_P_INT 4
// �������� �������������.
#define INFINITY INT_MAX

// ��������� ��������� �����.
struct Point
{
	p_int X, Y;
};

typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;