#include "Config.h"
#include "FieldSerializer.h"
#include "BasicTypes.h"
#include "Player.h"
#include "Field.h"
#include <string>
#include <fstream>

using namespace std;

bool FieldSerializer::ToXT(field &CurField, string FileName)
{
	if (CurField.width() != 39 || CurField.height() != 32 || CurField.get_sur_cond() != SC_STANDART || CurField.points_seq.size() == 0)
		return false;

	ofstream Stream(FileName, ios::binary | ios::out | ios::trunc);

	if (!Stream)
		return false;

	// ������ ���� - ������ �������.
	Stream << static_cast<char>(121);
	// ��������� 2 ����� - ���������� ������������ ����� - 1.
	Stream << static_cast<char>((CurField.points_seq.size() - 1) & 0xFF); Stream << static_cast<char>(((CurField.points_seq.size() - 1) >> 8) & 0xFF);
	// ����� 2 �����, ����������� �� ���� ���������� ������, ���������� ���.
	if (CurField.get_player(CurField.points_seq.back()) == player_red)
	{
		Stream << static_cast<char>(0xFF); Stream << static_cast<char>(0xFF);
	}
	else
	{
		Stream << static_cast<char>(0xFF);
	}
	// ???
	Stream << static_cast<char>(0x00); Stream << static_cast<char>(0x00);
	Stream << static_cast<char>(0x00); Stream << static_cast<char>(0x00);
	Stream << static_cast<char>(0x00); Stream << static_cast<char>(0x00);
	// ����� ���� ����� ���� ������� �� 9 ����.
	Stream << "                  ";
	// ������, ����� � ������ ������� ������ ���� ����� ���������� ������ ��� �� �����������������, ������ ����.
	for (ushort i = 0; i < 29; i++)
		Stream << static_cast<char>(0x00);
	for (auto i = CurField.points_seq.begin(); i < CurField.points_seq.end(); i++)
	{
		// ����� ���������� ���� - X, Y.
		coord x, y;
		CurField.to_xy(*i, x, y);
		Stream << static_cast<char>(x);
		Stream << static_cast<char>(y);
		// � ���� ����� ���������� ������������������ �����, �� ������� ������� ������� ����� ��� �������� ��������� (������� ���� � �������� ���� ���������). �� �������, ���� ����� �������� ���.
		Stream << static_cast<char>(1);
		// ����� ���� ������, ������������ �����.
		if (CurField.get_player(*i) == player_red)
		{
			Stream << static_cast<char>(0xFF); Stream << static_cast<char>(0xFF);
		}
		else
		{
			Stream << static_cast<char>(0x00); Stream << static_cast<char>(0x00);
		}
		// ???
		for (short j = 0; j < 8; j++)
			Stream << static_cast<char>(0x00);
	}

	return true;
}