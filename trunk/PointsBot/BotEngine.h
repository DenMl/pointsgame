#pragma once

#include "Field.h"

// Возвращает лучшую найденную тактическим анализом точку.
// Если не найдено - 0.
_int SearchBestMove(Field &MainField, _int Depth, _int UCTIterations);