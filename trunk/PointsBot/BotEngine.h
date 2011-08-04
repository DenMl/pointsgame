#pragma once

#include "Field.h"
#include "BasicTypes.h"

// Возвращает лучший найденный ход.
// Если не найдено - 0.
p_int SearchBestMove(Field &MainField, p_int Depth, p_int UCTIterations);

p_int SearchBestMoveWithTime(Field &MainField, p_int Time);