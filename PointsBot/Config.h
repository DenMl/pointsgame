#pragma once

// ������� ���������, ����� �� ����� �������������� ������� ��������� (������ ������������ ���) ��� ���������� ������������������.
#define SURROUND_CONDITIONS 1

// ������� ��������, ����� ������������ �������.
//#define DEBUG 1

// �������� ���������� �� ������������ ��� ��������� ���������-���������.
#define ALPHABETA_SORT 1

// ���������� ������� ������ ��� UCT - ���������� ��� ������������� (������ �� ������� �������) ������.
//#define ABSOLUTE_UCT 0

// 1, ���� ���������� ���������� ��� Windows.
#define WINDOWS 1

// 1, ���� ���������� ���������� ��� Linux.
#define LINUX 0

// ������������ ������� ���������. ������ ���� ������.
#define MAX_MINMAX_DEPTH 16

// ���������� ����������, ��� ������� ����� ���������� ������ � TrajectoryList. ���� �������������� ���������� ���������� ����� ��������� ��� ��������, ������ ����� ��������������, ��� ������� ������������ ������ �������.
#define MAX_TRAJECTORIES_COUNT 256

// ������������ ����� ����������. ��������������� ��� �������� ������������ ������� ���������.
#define MAX_TRAJECTORY_LENGTH (MAX_MINMAX_DEPTH / 2)