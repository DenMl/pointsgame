#pragma once

#include "Config.h"
#include "BasicTypes.h"
#include "Field.h"

using namespace std;

// ���������� ������ ��������� ���.
// ���� �� ������� - 0.
pos SearchBestMove(Field &MainField, uint Depth, ulong UCTIterations);
pos SearchBestMoveWithTime(Field &MainField, ulong Time);