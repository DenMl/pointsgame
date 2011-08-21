#include "Config.h"
#include "FieldSerializer.h"
#include "BasicTypes.h"
#include "Player.h"
#include "Field.h"
#include <string>
#include <fstream>

using namespace std;

bool FieldSerializer::ToXT(Field &CurField, string FileName)
{
	if (CurField.Width != 39 || CurField.Height != 32 || CurField.SurCond != Standart || CurField.PointsSeq.size() == 0)
		return false;

	ofstream Stream(FileName, ios_base::binary | ios_base::out | ios_base::trunc);

	if (!Stream)
		return false;

	// ������ ���� - ������ �������.
	Stream << static_cast<char>(121);
	// ��������� 2 ����� - ���������� ������������ ����� - 1.
	Stream << static_cast<ushort>(CurField.PointsSeq.size() - 1);
	// ����� 2 �����, ����������� �� ���� ���������� ������, ���������� ���.
	if (CurField.GetPlayer(CurField.PointsSeq.back()) == PlayerRed)
		Stream << static_cast<ushort>(0xFFFF);
	else
		Stream << static_cast<ushort>(0xFFFF);
	// ???
	Stream << static_cast<ushort>(0x0000);
	Stream << static_cast<ushort>(0x0000);
	Stream << static_cast<ushort>(0x0000);
	// ����� ���� ����� ���� ������� �� 9 ����.
	Stream << "                  ";
	// ������, ����� � ������ ������� ������ ���� ����� ���������� ������ ��� �� �����������������, ������ ����.
	for (ushort i = 0; i < 29; i++)
		Stream << static_cast<char>(0x00);
	for (auto i = CurField.PointsSeq.begin(); i < CurField.PointsSeq.end(); i++)
	{
		// ����� ���������� ���� - X, Y.
		coord x, y;
		CurField.ConvertToXY(*i, x, y);
		Stream << static_cast<char>(x);
		Stream << static_cast<char>(y);
		// � ���� ����� ���������� ������������������ �����, �� ������� ������� ������� ����� ��� �������� ��������� (������� ���� � �������� ���� ���������). �� �������, ���� ����� �������� ���.
		Stream << static_cast<char>(1);
		// ����� ���� ������, ������������ �����.
		if (CurField.GetPlayer(*i) == PlayerRed)
			Stream << static_cast<ushort>(0xFFFF);
		else
			Stream << static_cast<ushort>(0x0000);
		// ???
		for (short j = 0; j < 8; j++)
			Stream << static_cast<char>(0x00);
	}

	return true;
}