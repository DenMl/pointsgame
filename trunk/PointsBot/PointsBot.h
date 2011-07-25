#pragma once
#include "Field.h"

#define DLLEXPORT extern "C" __declspec(dllexport)

// Экспортируемая функция инициализации поля.
DLLEXPORT Field* InitField(p_int FieldWidth, p_int FieldHeight, p_int SurCond, p_int BeginPattern);
// Экспортируемя функция финализации поля.
DLLEXPORT void FinalField(Field *MainField);
// Экспортируемая функция постановки точки.
DLLEXPORT void PutPoint(Field *MainField, p_int X, p_int Y);
// Экспортируемая функция постановки точки игрока Player.
DLLEXPORT void PutPlayersPoint(Field *MainField, p_int X, p_int Y, p_int Player);
// Экспортируемая функция отката хода.
DLLEXPORT void RemoveLastPoint(Field *MainField);
// Экспортируемая функция получения наилучшего хода по мнению ИИ.
DLLEXPORT void GetBotMove(Field *MainField, p_int MinMaxDepth, p_int UCTIterations, p_int &X, p_int &Y);
DLLEXPORT void SetCurrentPlayer(Field *MainField, p_int Player);
DLLEXPORT p_int GetCurrentPlayer(Field *MainField);
DLLEXPORT void SetNextPlayer(Field *MainField);