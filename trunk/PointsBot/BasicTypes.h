#pragma once
#include <limits.h>

typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

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
	ushort X, Y;
};