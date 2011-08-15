#pragma once

#include "Config.h"
#include "BasicTypes.h"
#include "Field.h"

using namespace std;

// Возвращает лучший найденный ход.
// Если не найдено - 0.
uint SearchBestMove(Field &MainField, uint Depth, ulong UCTIterations);
uint SearchBestMoveWithTime(Field &MainField, ulong Time);