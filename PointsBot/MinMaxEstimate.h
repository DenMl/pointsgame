#pragma once
#include "BasicTypes.h"
#include "Field.h"

int MinMaxEstimate(Field &MainField, uint Depth, GameStack<uint, MAX_CHAIN_POINTS> &Moves);