#pragma once

#include "Config.h"
#include "basic_types.h"
#include "Field.h"
#include "static_vector.h"

using namespace std;

pos position_estimate(field &cur_field, list<pos> &moves);