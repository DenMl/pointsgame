#include "Config.h"
#include "Field.h"

void Field::PlaceBeginPattern()
{
	Point point;
	switch (Pattern)
	{
	case (CrosswisePattern):
		point.X = FieldWidth / 2 - 1;
		point.Y = FieldHeight / 2 - 1;
		DoStep(ConvertToPos(point));
		point.X++;
		DoStep(ConvertToPos(point));
		point.Y++;
		DoStep(ConvertToPos(point));
		point.X--;
		DoStep(ConvertToPos(point));
		break;
	case (SquarePattern):
		point.X = FieldWidth / 2 - 1;
		point.Y = FieldHeight / 2 - 1;
		DoStep(ConvertToPos(point));
		point.X++;
		DoStep(ConvertToPos(point));
		point.X--;
		point.Y++;
		DoStep(ConvertToPos(point));
		point.X++;
		DoStep(ConvertToPos(point));
		break;
	}
}

void Field::RemoveEmptyBase(const uint StartPos)
{
	Wave(	StartPos,
			[&](uint Pos)
			{
				return IsInEmptyBase(Pos);
			},
			[&](uint Pos)
			{
				Changes.back().Changes.push(Pair<uint, ushort>(Pos, Points[Pos] & !TagBit));
				ClearEmptyBase(Pos);
			});
}

bool Field::BuildChain(const uint StartPos, const short EnableCond, const uint DirectionPos, static_vector<uint, MAX_CHAIN_POINTS> &Chain)
{
	uint Pos;

	Chain.clear();
	Chain.push_back(StartPos);
	Pos = DirectionPos;
	uint CenterPos = StartPos;
	// Площадь базы.
	int TempSquare = Square(CenterPos, Pos);
	do
	{
		if (IsTagged(Pos))
		{
			while (Chain.back() != Pos)
			{
				ClearTag(Chain.back());
				Chain.pop_back();
			}
		}
		else
		{
			SetTag(Pos);
			Chain.push_back(Pos);
		}
		Swap(Pos, CenterPos);
		GetFirstNextPos(CenterPos, Pos);
		while (IsNotEnable(Pos, EnableCond))
			GetNextPos(CenterPos, Pos);
		TempSquare += Square(CenterPos, Pos);
	}
	while (Pos != StartPos);

	for (auto i = Chain.begin(); i < Chain.end(); i++)
		ClearTag(*i);

	return (TempSquare < 0 && Chain.size() > 2);
}

void Field::FindSurround(static_vector<uint, MAX_CHAIN_POINTS> &Chain, uint InsidePoint, short Player)
{
	// Количество захваченных точек.
	int CurCaptureCount = 0;
	// Количество захваченных пустых полей.
	int CurFreedCount = 0;

	static_vector<uint, MAX_CHAIN_POINTS> SurPoints;

	// Помечаем точки цепочки.
	for (auto i = Chain.begin(); i < Chain.end(); i++)
		SetTag(*i);

	Wave(	InsidePoint,
		[&, Player](uint Pos)
	{
		return IsNotBound(Pos, Player | PutBit | BoundBit);
	},
		[&, Player](uint Pos)
	{
		CheckCapturedAndFreed(Pos, Player, CurCaptureCount, CurFreedCount);
		SurPoints.push_back(Pos);
	});
	// Изменение счета игроков.
	AddSubCapturedFreed(Player, CurCaptureCount, CurFreedCount);
	DCaptureCount += CurCaptureCount;
	DFreedCount += CurFreedCount;

#if SURROUND_CONDITIONS
	if ((CurCaptureCount != 0) || (SurCond == Always)) // Если захватили точки, или стоит опция захватывать всегда.
#else
	if (CurCaptureCount != 0) // Если захватили точки.
#endif
	{
		for (auto i = Chain.begin(); i < Chain.end(); i++)
		{
			ClearTag(*i);
			// Добавляем в список изменений точки цепочки.
			Changes.back().Changes.push(Pair<uint, ushort>(*i, Points[*i]));
			// Помечаем точки цепочки.
			SetBaseBound(*i);
		}

		for (auto i = SurPoints.begin(); i < SurPoints.end(); i++)
		{
			Changes.back().Changes.push(Pair<uint, ushort>(*i, Points[*i]));

			SetCaptureFreeState(*i, Player);
		}
	}
	else // Если ничего не захватили.
	{
		for (auto i = Chain.begin(); i < Chain.end(); i++)
			ClearTag(*i);

		for (auto i = SurPoints.begin(); i < SurPoints.end(); i++)
		{
			Changes.back().Changes.push(Pair<uint, ushort>(*i, Points[*i]));

			if (!IsPutted(*i))
				SetEmptyBase(*i);
		}
	}
}

Field::Field(const ushort FieldWidth, const ushort FieldHeight, const SurroundCondition SurCond, const BeginPattern BeginPattern)
{
	ExpandWidth = (FieldWidth2 - FieldWidth) / 2;
	ParityWidth = (FieldWidth2 - FieldWidth) % 2;
	ExpandHeight = (FieldHeight2 - FieldHeight) / 2;
	ParityHeight = (FieldHeight2 - FieldHeight) % 2;

	// Верхнюю часть доски помечаем флагом BadValue.
	for (uint i = 0; i < ExpandHeight * FieldWidth2; i++)
		Points[i] = BadValue;

	uint Pos = (ExpandHeight - 1) * FieldWidth2 + ExpandWidth + FieldWidth;
	for (ushort i = 0; i < FieldHeight; i++)
	{
		// Боковые части доски помечаем флагом BadValue.
		for (ushort j = 0; j < ExpandWidth * 2 + ParityWidth; j++)
			Points[Pos++] = BadValue;
		// Очищаем центральную часть доски.
		for (ushort j = 0; j < FieldWidth; j++)
			Points[Pos++] = 0;
	}
	// Нижнюю часть доски (и нижнюю правую боковую часть) помечаем флагом BadValue.
	for (Pos; Pos < PointsLength22; Pos++)
		Points[Pos] = BadValue;

	this->FieldWidth = FieldWidth;
	this->FieldHeight = FieldHeight;
#if SURROUND_CONDITIONS
	this->SurCond = SurCond;
#endif
	this->Pattern = BeginPattern;
	CurPlayer = PlayerRed;
	CaptureCount[0] = 0;
	CaptureCount[1] = 0;

	Hash = 0;

	MinPos = UnsafeConvertToPos(0, 0);
	MaxPos = UnsafeConvertToPos(FieldWidth - 1, FieldHeight - 1);

	PlaceBeginPattern();
}

Field::Field(const Field &Orig)
{
	Changes.assign(Orig.Changes.begin(), Orig.Changes.end());

	ExpandWidth = Orig.ExpandWidth;
	ParityWidth = Orig.ParityWidth;
	ExpandHeight = Orig.ExpandHeight;
	ParityHeight = Orig.ParityHeight;

	for (uint i = 0; i < PointsLength22; i++)
		Points[i] = Orig.Points[i];

	FieldWidth = Orig.FieldWidth;
	FieldHeight = Orig.FieldHeight;
	PointsSeq.assign(Orig.PointsSeq.begin(), Orig.PointsSeq.end());
#if SURROUND_CONDITIONS
	SurCond = Orig.SurCond;
#endif
	Pattern = Orig.Pattern;
	CurPlayer = Orig.CurPlayer;
	CaptureCount[0] = Orig.CaptureCount[0];
	CaptureCount[1] = Orig.CaptureCount[1];

	Hash = Orig.Hash;

	MinPos = Orig.MinPos;
	MaxPos = Orig.MaxPos;
}