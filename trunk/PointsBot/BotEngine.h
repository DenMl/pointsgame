#pragma once

#include "Field.h"

// Возвращает лучшую найденную тактическим анализом точку.
// Если не найдено - 0.
p_int SearchBestMove(Field &MainField, p_int Depth, p_int UCTIterations);