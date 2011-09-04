#pragma once

#include "config.h"
#include "basic_types.h"

// Minimum supported board width.
// ����������� �������������� ������ �����.
const ushort MinFieldWidth = 1;
// Maximum supported board width.
// ������������ �������������� ������ �����.
const ushort MaxFieldWidth = 39;
// Minimum supported board height.
// ����������� �������������� ������ �����.
const ushort MinFieldHeight = 1;
// Maximum supported board height.
// ������������ �������������� ������ �����.
const ushort MaxFieldHeight = 32;

// ����������� ������� ����. ������� ��� ����������� ��������.
const ushort FieldWidth2 = 64;
const ushort FieldHeight2 = 64;
const uint PointsLength22 = FieldWidth2 * FieldHeight2;

// ������������ ���������� ����� �� ������ ����, ������� ����� ������������ �������.
const uint MAX_CHAIN_POINTS = MaxFieldWidth * MaxFieldHeight;
// ������������ ���������� ����� �� ������ ����, ������� ����� ���������� � ���������.
const uint MAX_SUR_POINTS = (MaxFieldWidth - 2) * (MaxFieldHeight - 2);