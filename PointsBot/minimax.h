#pragma once

#include "config.h"
#include "basic_types.h"
#include "field.h"
#include "trajectories.h"

using namespace std;

pos minimax(field* cur_field, size_t depth);