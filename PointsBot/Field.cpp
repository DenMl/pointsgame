#include "Config.h"
#include "Field.h"

using namespace std;

void Field::PlaceBeginPattern()
{
	ushort X, Y;
	switch (Pattern)
	{
	case (BP_CROSSWIRE):
		X = Width / 2 - 1;
		Y = Height / 2 - 1;
		DoStep(ConvertToPos(X, Y));
		X++;
		DoStep(ConvertToPos(X, Y));
		Y++;
		DoStep(ConvertToPos(X, Y));
		X--;
		DoStep(ConvertToPos(X, Y));
		break;
	case (BP_SQUARE):
		X = Width / 2 - 1;
		Y = Height / 2 - 1;
		DoStep(ConvertToPos(X, Y));
		X++;
		DoStep(ConvertToPos(X, Y));
		X--;
		Y++;
		DoStep(ConvertToPos(X, Y));
		X++;
		DoStep(ConvertToPos(X, Y));
		break;
	}
}

void Field::RemoveEmptyBase(const pos StartPos)
{
	Wave(	StartPos,
			[&](pos Pos)
			{
				return IsInEmptyBase(Pos);
			},
			[&](pos Pos)
			{
				Changes.back().changes.push(pair<pos, value>(Pos, Points[Pos] & !TagBit));
				ClearEmptyBase(Pos);
			});
}

bool Field::BuildChain(const pos StartPos, const value EnableCond, const pos DirectionPos, static_vector<pos, MAX_CHAIN_POINTS> &Chain)
{
	pos Pos;

	Chain.clear();
	Chain.push_back(StartPos);
	Pos = DirectionPos;
	pos CenterPos = StartPos;
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

void Field::FindSurround(static_vector<pos, MAX_CHAIN_POINTS> &Chain, pos InsidePoint, player Player)
{
	// Количество захваченных точек.
	int CurCaptureCount = 0;
	// Количество захваченных пустых полей.
	int CurFreedCount = 0;

	static_vector<pos, MAX_CHAIN_POINTS> SurPoints;

	// Помечаем точки цепочки.
	for (auto i = Chain.begin(); i < Chain.end(); i++)
		SetTag(*i);

	Wave(	InsidePoint,
		[&, Player](pos Pos)
	{
		return IsNotBound(Pos, Player | PutBit | BoundBit);
	},
		[&, Player](pos Pos)
	{
		CheckCapturedAndFreed(Pos, Player, CurCaptureCount, CurFreedCount);
		SurPoints.push_back(Pos);
	});
	// Изменение счета игроков.
	AddSubCapturedFreed(Player, CurCaptureCount, CurFreedCount);
	DCaptureCount += CurCaptureCount;
	DFreedCount += CurFreedCount;

#if SURROUND_CONDITIONS
	if ((CurCaptureCount != 0) || (SurCond == SC_ALWAYS)) // Если захватили точки, или стоит опция захватывать всегда.
#else
	if (CurCaptureCount != 0) // Если захватили точки.
#endif
	{
		for (auto i = Chain.begin(); i < Chain.end(); i++)
		{
			ClearTag(*i);
			// Добавляем в список изменений точки цепочки.
			Changes.back().changes.push(pair<pos, value>(*i, Points[*i]));
			// Помечаем точки цепочки.
			SetBaseBound(*i);
		}

		for (auto i = SurPoints.begin(); i < SurPoints.end(); i++)
		{
			Changes.back().changes.push(pair<pos, value>(*i, Points[*i]));

			SetCaptureFreeState(*i, Player);
		}
	}
	else // Если ничего не захватили.
	{
		for (auto i = Chain.begin(); i < Chain.end(); i++)
			ClearTag(*i);

		for (auto i = SurPoints.begin(); i < SurPoints.end(); i++)
		{
			Changes.back().changes.push(pair<pos, value>(*i, Points[*i]));

			if (!IsPutted(*i))
				SetEmptyBase(*i);
		}
	}
}

Field::Field(const coord FieldWidth, const coord FieldHeight, const sur_cond SurCond, const begin_pattern BeginPattern)
{
	ExpandWidth = (FieldWidth2 - FieldWidth) / 2;
	ParityWidth = (FieldWidth2 - FieldWidth) % 2;
	ExpandHeight = (FieldHeight2 - FieldHeight) / 2;
	ParityHeight = (FieldHeight2 - FieldHeight) % 2;

	// Верхнюю часть доски помечаем флагом BadValue.
	for (pos i = 0; i < ExpandHeight * FieldWidth2; i++)
		Points[i] = BadBit;

	pos Pos = (ExpandHeight - 1) * FieldWidth2 + ExpandWidth + FieldWidth;
	for (coord i = 0; i < FieldHeight; i++)
	{
		// Боковые части доски помечаем флагом BadValue.
		for (coord j = 0; j < ExpandWidth * 2 + ParityWidth; j++)
			Points[Pos++] = BadBit;
		// Очищаем центральную часть доски.
		for (coord j = 0; j < FieldWidth; j++)
			Points[Pos++] = 0;
	}
	// Нижнюю часть доски (и нижнюю правую боковую часть) помечаем флагом BadValue.
	for (Pos; Pos < PointsLength22; Pos++)
		Points[Pos] = BadBit;

	this->Width = FieldWidth;
	this->Height = FieldHeight;
#if SURROUND_CONDITIONS
	this->SurCond = SurCond;
#endif
	this->Pattern = BeginPattern;
	CurPlayer = PlayerRed;
	CaptureCount[0] = 0;
	CaptureCount[1] = 0;

	Hash = 0;

	MinPos = ConvertToPos(0, 0);
	MaxPos = ConvertToPos(FieldWidth - 1, FieldHeight - 1);

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

	Width = Orig.Width;
	Height = Orig.Height;
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