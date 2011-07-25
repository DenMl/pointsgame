#pragma once
#include "BasicTypes.h"

// Minimum supported board width.
// ����������� �������������� ������ �����.
static const p_int MinFieldWidth = 1;
// Maximum supported board width.
// ������������ �������������� ������ �����.
static const p_int MaxFieldWidth = 39;
// Minimum supported board height.
// ����������� �������������� ������ �����.
static const p_int MinFieldHeight = 1;
// Maximum supported board height.
// ������������ �������������� ������ �����.
static const p_int MaxFieldHeight = 32;

// ����������� ������� ����. ������� ��� ����������� ��������.
static const p_int FieldWidth2 = 64;
static const p_int FieldHeight2 = 64;
static const p_int PointsLength22 = FieldWidth2 * FieldHeight2;

// ������������ ���������� ����� �� ������ ����, ������� ����� ������������ �������.
static const p_int MAX_CHAIN_POINTS = MaxFieldWidth * MaxFieldHeight;
// ������������ ���������� ����� �� ������ ����, ������� ����� ���������� � ���������.
static const p_int MAX_SUR_POINTS = (MaxFieldWidth - 2) * (MaxFieldHeight - 2);