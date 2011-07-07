#pragma once
#include "Field.h"

#define DLLEXPORT extern "C" __declspec(dllexport)

// �������������� ������� ������������� ����.
DLLEXPORT Field* InitField(_int FieldWidth, _int FieldHeight, _int SurCond, _int BeginPattern);
// ������������� ������� ����������� ����.
DLLEXPORT void FinalField(Field *MainField);
// �������������� ������� ���������� �����.
DLLEXPORT void PutPoint(Field *MainField, _int X, _int Y);
// �������������� ������� ���������� ����� ������ Player.
DLLEXPORT void PutPlayersPoint(Field *MainField, _int X, _int Y, _int Player);
// �������������� ������� ������ ����.
DLLEXPORT void RemoveLastPoint(Field *MainField);
// �������������� ������� ��������� ���������� ���� �� ������ ��.
DLLEXPORT void GetBotMove(Field *MainField, _int MinMaxDepth, _int UCTIterations, _int &X, _int &Y);
DLLEXPORT void SetCurrentPlayer(Field *MainField, _int Player);
DLLEXPORT _int GetCurrentPlayer(Field *MainField);
DLLEXPORT void SetNextPlayer(Field *MainField);