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
	if (CurField.Width != 39 || CurField.Height != 32 || CurField.SurCond != SC_STANDART || CurField.PointsSeq.size() == 0)
		return false;

	ofstream Stream(FileName, ios::binary | ios::out | ios::trunc);

	if (!Stream)
		return false;

	// ѕервый байт - верси€ клиента.
	Stream << static_cast<char>(121);
	// —ледующие 2 байта - количество поставленных точек - 1.
	Stream << static_cast<char>((CurField.PointsSeq.size() - 1) & 0xFF); Stream << static_cast<char>(((CurField.PointsSeq.size() - 1) >> 8) & 0xFF);
	// ƒалее 2 байта, указывающие на цвет последнего игрока, сделавшего ход.
	if (CurField.GetPlayer(CurField.PointsSeq.back()) == PlayerRed)
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
	// ƒалее идут имена двух игроков по 9 байт.
	Stream << "                  ";
	// ¬идимо, здесь в первых четырех байтах идет врем€ сохранени€ партии или ее продолжительность, дальше нули.
	for (ushort i = 0; i < 29; i++)
		Stream << static_cast<char>(0x00);
	for (auto i = CurField.PointsSeq.begin(); i < CurField.PointsSeq.end(); i++)
	{
		// ƒалее координаты хода - X, Y.
		coord x, y;
		CurField.ConvertToXY(*i, x, y);
		Stream << static_cast<char>(x);
		Stream << static_cast<char>(y);
		// ¬ этом байте помечаетс€ последовательность точек, от которых следует пускать волну дл€ проверки окружений (которые были в процессе игры захвачены). Ќе страшно, если будут помечены все.
		Stream << static_cast<char>(1);
		// «атем цвет игрока, поставившего точку.
		if (CurField.GetPlayer(*i) == PlayerRed)
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