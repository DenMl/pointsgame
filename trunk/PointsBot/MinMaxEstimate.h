#pragma once
#include "BasicTypes.h"
#include "Field.h"

p_int MinMaxEstimate(Field &MainField, p_int Depth, GameStack<p_int, MAX_CHAIN_POINTS> &Moves);