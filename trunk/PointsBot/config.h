#pragma once

// ������� ��������� ������ ���.
// STANDART = 0 - ���� PlayerRed ������ � ������ ���� � ������ �� �������, �� PlayerBlack ������� ��� ����������.
// ALWAYS = 1 - �������� ����, ���� ���� ��� ��������� ����� ������.
// ALWAYS_ENEMY = 2 - ������� ������ PlayerBlack, ���� PlayerRed �������� ����� � ������ ����.
#define SUR_COND 0

// �������� ���������� �� ������������ ��� ��������� ���������-���������.
#define ALPHABETA_SORT 1

// Larger values give uniform search.
// Smaller values give very selective search.
#define UCTK 1

// ������, ������ �������� ���������� ������ UCT.
#define UCT_RADIUS 2

// ���������� �������� uct ����� ��������� �������.
#define UCT_ITERATIONS_BEFORE_CHECK_TIME 100

// 1, ���� ���������� ���������� ��� Windows.
#define WINDOWS 1

// 1, ���� ���������� ���������� ��� Linux.
#define LINUX 0

#define X64 0