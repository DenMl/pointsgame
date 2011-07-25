#pragma once
#include "Field.h"

#define DLLEXPORT extern "C" __declspec(dllexport)

// �������������� ������� ������������� ����.
DLLEXPORT Field* InitField(p_int FieldWidth, p_int FieldHeight, p_int SurCond, p_int BeginPattern);
// ������������� ������� ����������� ����.
DLLEXPORT void FinalField(Field *MainField);
// �������������� ������� ���������� �����.
DLLEXPORT void PutPoint(Field *MainField, p_int X, p_int Y);
// �������������� ������� ���������� ����� ������ Player.
DLLEXPORT void PutPlayersPoint(Field *MainField, p_int X, p_int Y, p_int Player);
// �������������� ������� ������ ����.
DLLEXPORT void RemoveLastPoint(Field *MainField);
// �������������� ������� ��������� ���������� ���� �� ������ ��.
DLLEXPORT void GetBotMove(Field *MainField, p_int MinMaxDepth, p_int UCTIterations, p_int &X, p_int &Y);
DLLEXPORT void SetCurrentPlayer(Field *MainField, p_int Player);
DLLEXPORT p_int GetCurrentPlayer(Field *MainField);
DLLEXPORT void SetNextPlayer(Field *MainField);