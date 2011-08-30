#pragma once

#include "Config.h"

using namespace std;

// Возвращает модуль числа x.
template<typename TYPE> inline TYPE Abs(TYPE x)
{
	return x >= 0 ? x : -x;
}