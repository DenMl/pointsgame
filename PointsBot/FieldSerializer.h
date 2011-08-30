#pragma once

#include "Config.h"
#include "Field.h"
#include <string>

using namespace std;

class FieldSerializer
{
public:
	static bool ToXT(field &CurField, string FileName);
};