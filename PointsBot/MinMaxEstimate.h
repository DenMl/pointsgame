#pragma once

#include "Config.h"
#include "BasicTypes.h"
#include "Field.h"
#include "static_vector.h"

int MinMaxEstimate(Field &MainField, uint Depth, static_vector<uint, MAX_CHAIN_POINTS> &Moves);