#pragma once

#include "Config.h"
#include "Field.h"

#define DLLEXPORT extern "C" __declspec(dllexport)

// �������������� ������� ������������� ����.
DLLEXPORT Field* InitField(size_t FieldWidth, size_t FieldHeight, size_t SurCond, size_t Pattern);
// ������������� ������� ����������� ����.
DLLEXPORT void FinalField(Field *MainField);
// �������������� ������� ���������� �����.
DLLEXPORT void PutPoint(Field *MainField, size_t X, size_t Y);
// �������������� ������� ���������� ����� ������ Player.
DLLEXPORT void PutPlayersPoint(Field *MainField, size_t X, size_t Y, size_t Player);
// �������������� ������� ������ ����.
DLLEXPORT void RemoveLastPoint(Field *MainField);
// �������������� ������� ��������� ���������� ���� �� ������ ��.
DLLEXPORT void GetBotMove(Field *MainField, size_t MinMaxDepth, size_t UCTIterations, short &X, short &Y);
DLLEXPORT void SetCurrentPlayer(Field *MainField, size_t Player);
DLLEXPORT size_t GetCurrentPlayer(Field *MainField);
DLLEXPORT void SetNextPlayer(Field *MainField);