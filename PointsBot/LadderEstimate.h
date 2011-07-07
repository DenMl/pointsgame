#pragma once
#include "BasicTypes.h"
#include "Field.h"

void LadderExcludeEstimate(Field &MainField, GameStack<_int, MAX_CHAIN_POINTS> &Moves);
_int LadderIncludeEstimate(Field &MainField, GameStack<_int, MAX_CHAIN_POINTS> &Moves);