#pragma once

#include "Config.h"
#include "BasicTypes.h"
#include "Field.h"
#include "static_vector.h"

using namespace std;

void PositionEstimate(field &MainField, static_vector<pos, MAX_CHAIN_POINTS> &Moves);