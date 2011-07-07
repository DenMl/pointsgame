#pragma once
#include "BasicTypes.h"

// Minimum supported board width.
// ����������� �������������� ������ �����.
static const _int MinFieldWidth = 1;
// Maximum supported board width.
// ������������ �������������� ������ �����.
static const _int MaxFieldWidth = 39;
// Minimum supported board height.
// ����������� �������������� ������ �����.
static const _int MinFieldHeight = 1;
// Maximum supported board height.
// ������������ �������������� ������ �����.
static const _int MaxFieldHeight = 32;

// ����������� ������� ����. ������� ��� ����������� ��������.
static const _int FieldWidth2 = 64;
static const _int FieldHeight2 = 64;
static const _int PointsLength22 = FieldWidth2 * FieldHeight2;

// ������������ ���������� ����� �� ������ ����, ������� ����� ������������ �������.
static const _int MAX_CHAIN_POINTS = MaxFieldWidth * MaxFieldHeight;
// ������������ ���������� ����� �� ������ ����, ������� ����� ���������� � ���������.
static const _int MAX_SUR_POINTS = (MaxFieldWidth - 2) * (MaxFieldHeight - 2);