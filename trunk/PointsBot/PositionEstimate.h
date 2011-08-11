#pragma once

#include "Config.h"
#include "BasicTypes.h"
#include "Field.h"
#include "static_vector.h"

void PositionEstimate(Field &MainField, static_vector<uint, MAX_CHAIN_POINTS> &Moves);