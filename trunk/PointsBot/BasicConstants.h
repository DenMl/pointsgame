#pragma once

#include "Config.h"
#include "BasicTypes.h"

// Minimum supported board width.
// ����������� �������������� ������ �����.
static const ushort MinFieldWidth = 1;
// Maximum supported board width.
// ������������ �������������� ������ �����.
static const ushort MaxFieldWidth = 39;
// Minimum supported board height.
// ����������� �������������� ������ �����.
static const ushort MinFieldHeight = 1;
// Maximum supported board height.
// ������������ �������������� ������ �����.
static const ushort MaxFieldHeight = 32;

// ����������� ������� ����. ������� ��� ����������� ��������.
static const ushort FieldWidth2 = 64;
static const ushort FieldHeight2 = 64;
static const uint PointsLength22 = FieldWidth2 * FieldHeight2;

// ������������ ���������� ����� �� ������ ����, ������� ����� ������������ �������.
static const uint MAX_CHAIN_POINTS = MaxFieldWidth * MaxFieldHeight;
// ������������ ���������� ����� �� ������ ����, ������� ����� ���������� � ���������.
static const uint MAX_SUR_POINTS = (MaxFieldWidth - 2) * (MaxFieldHeight - 2);