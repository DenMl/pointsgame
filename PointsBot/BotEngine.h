#pragma once

#include "Config.h"
#include "BasicTypes.h"
#include "Field.h"

using namespace std;

// ���������� ������ ��������� ���.
// ���� �� ������� - 0.
pos SearchBestMove(field &MainField, uint Depth, ulong UCTIterations);
pos SearchBestMoveWithTime(field &MainField, ulong Time);