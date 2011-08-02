#pragma once
#include "BasicTypes.h"

// �������� ���� ������� ������.
static const p_int PlayerRed = 0x0;
// �������� ���� ������� ������.
static const p_int PlayerBlack = 0x1;

// �������� �� ������ ���������� ������.
inline p_int NextPlayer(const p_int Player)
{
	return Player ^ 1;
}