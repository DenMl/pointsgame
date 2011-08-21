#pragma once

#include "Config.h"
#include "Field.h"

#define DLLEXPORT extern "C" __declspec(dllexport)

// Экспортируемая функция инициализации поля.
DLLEXPORT Field* InitField(size_t FieldWidth, size_t FieldHeight, size_t SurCond, size_t Pattern);
// Экспортируемя функция финализации поля.
DLLEXPORT void FinalField(Field *MainField);
// Экспортируемая функция постановки точки.
DLLEXPORT void PutPoint(Field *MainField, size_t X, size_t Y);
// Экспортируемая функция постановки точки игрока Player.
DLLEXPORT void PutPlayersPoint(Field *MainField, size_t X, size_t Y, size_t Player);
// Экспортируемая функция отката хода.
DLLEXPORT void RemoveLastPoint(Field *MainField);
// Экспортируемая функция получения наилучшего хода по мнению ИИ.
DLLEXPORT void GetBotMove(Field *MainField, size_t MinMaxDepth, size_t UCTIterations, short &X, short &Y);
DLLEXPORT void SetCurrentPlayer(Field *MainField, size_t Player);
DLLEXPORT size_t GetCurrentPlayer(Field *MainField);
DLLEXPORT void SetNextPlayer(Field *MainField);