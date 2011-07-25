#pragma once
#include "BasicTypes.h"
#include "Field.h"

void LadderExcludeEstimate(Field &MainField, GameStack<p_int, MAX_CHAIN_POINTS> &Moves);
p_int LadderIncludeEstimate(Field &MainField, GameStack<p_int, MAX_CHAIN_POINTS> &Moves);