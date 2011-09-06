#pragma once

#include "config.h"
#include "field.h"
#include <string>

using namespace std;

class field_serializer
{
public:
	static bool to_xt(field &CurField, string FileName);
};