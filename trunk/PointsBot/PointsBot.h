#pragma once
#include "Field.h"

#define DLLEXPORT extern "C" __declspec(dllexport)

// Экспортируемая функция инициализации поля.
DLLEXPORT Field* InitField(ushort FieldWidth, ushort FieldHeight, SurroundCondition SurCond, BeginPattern Pattern);
// Экспортируемя функция финализации поля.
DLLEXPORT void FinalField(Field *MainField);
// Экспортируемая функция постановки точки.
DLLEXPORT void PutPoint(Field *MainField, ushort X, ushort Y);
// Экспортируемая функция постановки точки игрока Player.
DLLEXPORT void PutPlayersPoint(Field *MainField, ushort X, ushort Y, short Player);
// Экспортируемая функция отката хода.
DLLEXPORT void RemoveLastPoint(Field *MainField);
// Экспортируемая функция получения наилучшего хода по мнению ИИ.
DLLEXPORT void GetBotMove(Field *MainField, uint MinMaxDepth, ulong UCTIterations, ushort &X, ushort &Y);
DLLEXPORT void SetCurrentPlayer(Field *MainField, short Player);
DLLEXPORT short GetCurrentPlayer(Field *MainField);
DLLEXPORT void SetNextPlayer(Field *MainField);