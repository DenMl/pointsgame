#pragma once

#include "Config.h"
#include "Field.h"

#define DLLEXPORT extern "C" __declspec(dllexport)

// �������������� ������� ������������� ����.
DLLEXPORT field* InitField(size_t FieldWidth, size_t FieldHeight, size_t SurCond, size_t Pattern);
// ������������� ������� ����������� ����.
DLLEXPORT void FinalField(field *MainField);
// �������������� ������� ���������� �����.
DLLEXPORT void PutPoint(field *MainField, size_t X, size_t Y);
// �������������� ������� ���������� ����� ������ Player.
DLLEXPORT void PutPlayersPoint(field *MainField, size_t X, size_t Y, size_t Player);
// �������������� ������� ������ ����.
DLLEXPORT void RemoveLastPoint(field *MainField);
// �������������� ������� ��������� ���������� ���� �� ������ ��.
DLLEXPORT void GetBotMove(field *MainField, size_t MinMaxDepth, size_t UCTIterations, short &X, short &Y);
DLLEXPORT void SetCurrentPlayer(field *MainField, size_t Player);
DLLEXPORT size_t GetCurrentPlayer(field *MainField);
DLLEXPORT void SetNextPlayer(field *MainField);