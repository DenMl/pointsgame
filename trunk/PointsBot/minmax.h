#pragma once

#include "config.h"
#include "basic_types.h"
#include "field.h"

using namespace std;

pos minmax(field &cur_field, size_t depth, list<pos> &moves);