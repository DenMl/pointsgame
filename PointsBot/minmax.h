#pragma once

#include "Config.h"
#include "basic_types.h"
#include "Field.h"

using namespace std;

pos minmax(field &cur_field, size_t depth, list<pos> &moves);