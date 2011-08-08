#pragma once
#include "Field.h"

#define DLLEXPORT extern "C" __declspec(dllexport)

// �������������� ������� ������������� ����.
DLLEXPORT Field* InitField(ushort FieldWidth, ushort FieldHeight, SurroundCondition SurCond, BeginPattern Pattern);
// ������������� ������� ����������� ����.
DLLEXPORT void FinalField(Field *MainField);
// �������������� ������� ���������� �����.
DLLEXPORT void PutPoint(Field *MainField, ushort X, ushort Y);
// �������������� ������� ���������� ����� ������ Player.
DLLEXPORT void PutPlayersPoint(Field *MainField, ushort X, ushort Y, short Player);
// �������������� ������� ������ ����.
DLLEXPORT void RemoveLastPoint(Field *MainField);
// �������������� ������� ��������� ���������� ���� �� ������ ��.
DLLEXPORT void GetBotMove(Field *MainField, uint MinMaxDepth, ulong UCTIterations, ushort &X, ushort &Y);
DLLEXPORT void SetCurrentPlayer(Field *MainField, short Player);
DLLEXPORT short GetCurrentPlayer(Field *MainField);
DLLEXPORT void SetNextPlayer(Field *MainField);