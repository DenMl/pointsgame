#pragma once
#include "BasicTypes.h"

// �������� ���� ������� ������.
static const short PlayerRed = 0x0;
// �������� ���� ������� ������.
static const short PlayerBlack = 0x1;

// �������� �� ������ ���������� ������.
inline short NextPlayer(const short Player)
{
	return Player ^ 1;
}