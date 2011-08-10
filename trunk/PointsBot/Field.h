#pragma once

#include "Config.h"
#include "BasicTypes.h"
#include "BasicConstants.h"
#include "Zobrist.h"
#include "AuxFunc.h"
#include "Player.h"
#include <vector>
#include <queue>
#include <stack>
#include <list>
#include <functional>

using namespace std;
using std::tr1::function;

class Field
{
#pragma region State bits and masks
	private:

	// Бит, указывающий номер игрока.
	static const ushort PlayerBit = 0x1;
	// Бит, указывающий на наличие точки в поле.
	static const ushort PutBit = 0x2;
	// Бит, указывающий на захваченность точки.
	static const ushort SurBit = 0x4;
	// Бит, указывающий на то, захватывает ли что-нибудь точка на поле.
	static const ushort BoundBit = 0x8;
	// Бит, указывающий на пустую базу.
	static const ushort EmptyBaseBit = 0x10;
	// Бит для временных пометок полей.
	static const ushort TagBit = 0x20;
	// Бит, которым помечаются границы поля.
	static const ushort BadValue = 0x40;

	// Маски для определения условий.
	static const ushort EnableMask = BadValue | SurBit | PutBit | PlayerBit;
	static const ushort BoundMask = BadValue | BoundBit | SurBit | PutBit | PlayerBit;
#pragma endregion

#pragma region Get and set state functions
	public:
	// Get state functions.
	// Функции получения состояния.

	// Получить по координате игрока, чья точка там поставлена.
	inline const short GetPlayer(const uint Pos) { return Points[Pos] & PlayerBit; }
	// Проверить по координате, поставлена ли там точка.
	inline const bool IsPutted(const uint Pos) { return (Points[Pos] & PutBit) != 0; }
	// Прверить по координате, является ли точка окружающей базу.
	inline const bool IsBaseBound(const uint Pos) { return (Points[Pos] & BoundBit) != 0; }
	// Проверить по координате, захвачено ли поле.
	inline const bool IsCaptured(const uint Pos) { return (Points[Pos] & SurBit) != 0; }
	// Проверить по координате, лежит ли она в пустой базе.
	inline const bool IsInEmptyBase(const uint Pos) { return (Points[Pos] & EmptyBaseBit) != 0; }
	// Проверить по координате, помечено ли поле.
	inline const bool IsTagged(const uint Pos) { return (Points[Pos] & TagBit) != 0; }
	// Получить условие по координате.
	inline const ushort GetEnableCond(const uint Pos) { return Points[Pos] & EnableMask; }
	// Проверка незанятости поля по условию.
	inline const bool IsEnable(const uint Pos, const ushort EnableCond) { return (Points[Pos] & EnableMask) == EnableCond; }
	// Проверка занятости поля по условию.
	inline const bool IsNotEnable(const uint Pos, const ushort EnableCond) { return (Points[Pos] & EnableMask) != EnableCond; }
	// Проверка на то, захвачено ли поле.
	inline const bool IsBound(const uint Pos, const ushort BoundCond) { return (Points[Pos] & BoundMask) == BoundCond; }
	// Проверка на то, не захвачено ли поле.
	inline const bool IsNotBound(const uint Pos, const ushort BoundCond) { return (Points[Pos] & BoundMask) != BoundCond; }
	// Провека на то, возможно ли поставить точку в полке.
	inline const bool PuttingAllow(const uint Pos) { return !(Points[Pos] & (PutBit | SurBit | BadValue)); }

	// Set state functions.
	// Функции установки состояния.

	// Пометить поле по координате как содержащее точку.
	inline void SetPutted(const uint Pos) { Points[Pos] |= PutBit; }
	// Убрать с поля по координате PutBit.
	inline void ClearPutBit(const uint Pos) { Points[Pos] &= ~PutBit; }
	// Пометить поле по координате как принадлежащее игроку.
	inline void SetPlayer(const uint Pos, const short Player) { Points[Pos] = (Points[Pos] & ~PlayerBit) | Player; }
	// Пометить поле по координате как содержащее точку игрока.
	inline void SetPlayerPutted(const uint Pos, const short Player) { Points[Pos] |= Player | PutBit; }
	// Пометить битом SurBit (захватить).
	inline void Capture(const uint Pos) { Points[Pos] |= SurBit; }
	// Убрать бит SurBit.
	inline void Free(const uint Pos) { Points[Pos] &= ~SurBit; }
	// Пометить точку как окружающую базу.
	inline void SetBaseBound(const uint Pos) { Points[Pos] |= BoundBit; }
	// Пометить точку как не окружающую базу.
	inline void ClearBaseBound(const uint Pos) { Points[Pos] &= ~BoundBit; }
	inline void SetInEmptyBase(const uint Pos, const short Player) { Points[Pos] &= ~PlayerBit | EmptyBaseBit | Player; }
	inline void SetEmptyBase(const uint Pos) { Points[Pos] |= EmptyBaseBit; }
	inline void ClearEmptyBase(const uint Pos) { Points[Pos] &= ~EmptyBaseBit; }
	// Установить бит TagBit.
	inline void SetTag(const uint Pos) { Points[Pos] |= TagBit; }
	// Убрать бит TagBit.
	inline void ClearTag(const uint Pos) { Points[Pos] &= ~TagBit; }
#pragma endregion

#pragma region MainVariables
	public:
	vector<BoardChange> Changes;

	// Main points array (game board).
	// Основной массив точек (игровая доска).
	ushort Points[PointsLength22];

	// Real field width.
	// Действительная ширина поля.
	ushort FieldWidth;
	// Real field height.
	// Действительная высота поля.
	ushort FieldHeight;

	// History points sequance.
	// Последовательность поставленных точек.
	vector<uint> PointsSeq;

	// Правила обработки пустых баз.
	// SurStandart = 0 - если PlayerRed ставит в пустую базу и ничего не обводит, то PlayerBlack обводит эту территорию.
	// SurAlways = 1 - обводить базу, даже если нет вражеских точек внутри.
	// SurAlwaysEnemy = 2 - обводит всегда PlayerBlack, если PlayerRed поставил точку в пустую базу.
	SurroundCondition SurCond;

	// Используемый шаблон в начале игры.
	// Crosswise = 0 - Crosswise.
	// Clean = 1 - Clean.
	// Square = 2 - Square.
	BeginPattern Pattern;

	// Размеры границ поля.
	ushort ExpandWidth, ExpandHeight;
	// Четность размеров поля (четные => 0; нечетные => 1).
	ushort ParityWidth, ParityHeight;

	// Current player color.
	// Текущий цвет игроков.
	short CurPlayer;

	// Capture points count.
	// Количество захваченных точек.
	uint CaptureCount[2];

	int DCaptureCount, DFreedCount;

	ulong Hash;

	public:
	// Координаты, соответствующие левому верхнему и правому нижнему углам доски.
	uint MinPos, MaxPos;
#pragma endregion

private:
	// Возвращает косое произведение векторов Pos1 и Pos2.
	inline static int Square(const uint Pos1, const uint Pos2);
	//  * . .   x . *   . x x   . . .
	//  . o .   x o .   . o .   . o x
	//  x x .   . . .   . . *   * . x
	//  o - center pos
	//  x - pos
	//  * - result
	inline const void GetFirstNextPos(const uint CenterPos, uint &Pos);
	//  . . .   * . .   x * .   . x *   . . x   . . .   . . .   . . .
	//  * o .   x o .   . o .   . o .   . o *   . o x   . o .   . o .
	//  x . .   . . .   . . .   . . .   . . .   . . *   . * x   * x .
	//  o - center pos
	//  x - pos
	//  * - result
	inline const void GetNextPos(const uint CenterPos, uint &Pos);
	// Возвращает количество групп точек рядом с CenterPos.
	// InpChainPoints - возможные точки цикла, InpSurPoints - возможные окруженные точки.
	inline const ushort GetInputPoints(const uint CenterPos, const ushort EnableCond, uint InpChainPoints[], uint InpSurPoints[]);
	// Поставить начальные точки.
	void PlaceBeginPattern();
	// Изменение счета игроков.
	inline void AddSubCapturedFreed(const short Player, const int Captured, const int Freed);
	// Изменяет Captured/Free в зависимости от того, захвачена или окружена точка.
	inline void CheckCapturedAndFreed(const uint Pos, const short Player, int &Captured, int &Freed);
	// Захватывает/освобождает окруженную точку.
	inline void SetCaptureFreeState(const uint Pos, const short Player);
	inline void Wave(uint StartPos, function<bool(int)> Condition, function<void(uint)> Action);
	// Удаляет пометку пустой базы с поля точек, начиная с позиции StartPos.
	void RemoveEmptyBase(const uint StartPos);
	bool BuildChain(const uint StartPos, short EnableCond, const uint DirectionPos, list<uint> &Chain);
	void FindSurround(list<uint> &Chain, uint InsidePoint, short Player);
	inline void UpdateHash(short Player, short Surrounded, uint Pos);
	inline const IntersectionState GetIntersectionState(const uint Pos, const uint NextPos);

public:
	// Конструктор.
	Field(const ushort FieldWidth, const ushort FieldHeight, const SurroundCondition SurCond, const BeginPattern BeginPattern);
	// Конструктор копирования.
	Field(const Field &Orig);

	inline const int GetScore(short Player)
	{
		return CaptureCount[Player] - CaptureCount[NextPlayer(Player)];
	}

	// Функции конвертации координат.
	inline const uint ConvertToPos(const ushort X, const ushort Y)
	{
		if ((X < 0) || (X >= FieldWidth) || (Y < 0) || (Y >= FieldHeight))
			return 0;
		else
			return (Y + ExpandHeight) * FieldWidth2 + X + ExpandWidth;
	}
	inline const uint ConvertToPos(const Point point)
	{
		return ConvertToPos(point.X, point.Y);
	}
	// Небезопасные функции конвертации координат.
	inline const uint UnsafeConvertToPos(const ushort X, const ushort Y)
	{
		return (Y + ExpandHeight) * FieldWidth2 + X + ExpandWidth;
	}
	inline const uint UnsafeConvertToPos(const Point point)
	{
		return UnsafeConvertToPos(point.X, point.Y);
	}
	// Конвертация из Pos в XY.
	inline const void ConvertToXY(const uint Pos, ushort &X, ushort &Y)
	{
		X = Pos % FieldWidth2 - ExpandWidth;
		Y = Pos / FieldWidth2 - ExpandHeight;
	}
	inline const void ConvertToXY(const uint Pos, Point &P)
	{
		P.X = Pos % FieldWidth2 - ExpandWidth;
		P.Y = Pos / FieldWidth2 - ExpandHeight;
	}

	// Поставить точку на поле следующего по очереди игрока.
	inline bool DoStep(const uint Pos)
	{
		if (PuttingAllow(Pos))
		{
			DoUnsafeStep(Pos);
			return true;
		}
		return false;
	}
	// Поставить точку на поле.
	inline bool DoStep(const uint Pos, const short Player)
	{
		if (PuttingAllow(Pos))
		{
			DoUnsafeStep(Pos, Player);
			return true;
		}
		return false;
	}
	// Поставить точку на поле максимально быстро (без дополнительных проверок).
	inline void DoUnsafeStep(const uint Pos)
	{
		Changes.resize(Changes.size() + 1);
		Changes.back().CaptureCount[0] = CaptureCount[0];
		Changes.back().CaptureCount[1] = CaptureCount[1];
		Changes.back().Player = CurPlayer;

		// Добавляем в изменения поставленную точку.
		Changes.back().Changes.push(Pair<uint, ushort>(Pos, Points[Pos]));

		SetPlayer(Pos, CurPlayer);
		SetPutted(Pos);

		PointsSeq.push_back(Pos);

		CheckClosure(Pos, CurPlayer);

		SetNextPlayer();
	}
	// Поставить точку на поле следующего по очереди игрока максимально быстро (без дополнительных проверок).
	inline void DoUnsafeStep(const uint Pos, const short Player)
	{
		Changes.resize(Changes.size() + 1);
		Changes.back().CaptureCount[0] = CaptureCount[0];
		Changes.back().CaptureCount[1] = CaptureCount[1];
		Changes.back().Player = CurPlayer;

		// Добавляем в изменения поставленную точку.
		Changes.back().Changes.push(Pair<uint, ushort>(Pos, Points[Pos]));

		SetPlayer(Pos, Player);
		SetPutted(Pos);

		PointsSeq.push_back(Pos);

		CheckClosure(Pos, Player);
	}

	// Делает ход и проверяет на окруженность только точку CheckedPos.
	inline bool DoUnsafeStepAndCheckPoint(const uint Pos, const short Player, const uint CheckedPos)
	{
		Changes.resize(Changes.size() + 1);
		Changes.back().CaptureCount[0] = CaptureCount[0];
		Changes.back().CaptureCount[1] = CaptureCount[1];
		Changes.back().Player = CurPlayer;

		// Добавляем в изменения поставленную точку.
		Changes.back().Changes.push(Pair<uint, ushort>(Pos, Points[Pos]));

		SetPlayer(Pos, Player);
		SetPutted(Pos);

		PointsSeq.push_back(Pos);
		
		return CheckClosure(Pos, CheckedPos);
	}

	// Откат хода.
	inline void UndoStep()
	{
		PointsSeq.pop_back();
		while (!Changes.back().Changes.empty())
		{
			Points[Changes.back().Changes.top().first] = Changes.back().Changes.top().second;
			Changes.back().Changes.pop();
		}
		CurPlayer = Changes.back().Player;
		CaptureCount[0] = Changes.back().CaptureCount[0];
		CaptureCount[1] = Changes.back().CaptureCount[1];
		Changes.pop_back();
	}

	// Установить следующего игрока как текущего.
	inline void SetNextPlayer()
	{
		CurPlayer = NextPlayer(CurPlayer);
	}

	inline void SetCurrentPlayer(const short Player)
	{
		CurPlayer = Player;
	}

	// Проверяет, находятся ли две точки рядом.
	inline static bool IsNear(const uint Pos1, const uint Pos2)
	{
		uint TempPos = Pos1 - FieldWidth2 - 1;
		if (TempPos == Pos2)
			return true;
		TempPos++;
		if (TempPos == Pos2)
			return true;
		TempPos++;
		if (TempPos == Pos2)
			return true;
		TempPos += FieldWidth2;
		if (TempPos == Pos2)
			return true;
		TempPos -= 2;
		if (TempPos == Pos2)
			return true;
		TempPos +=FieldWidth2;
		if (TempPos == Pos2)
			return true;
		TempPos++;
		if (TempPos == Pos2)
			return true;
		TempPos++;
		if (TempPos == Pos2)
			return true;
		return false;
	}

	// Проверяет, есть ли рядом с Pos точки цвета Player.
	inline const bool IsNearPoints(const uint Pos, const short Player)
	{
		uint TempPos = Pos - FieldWidth2 - 1;
		if (IsEnable(TempPos, PutBit | Player))
			return true;
		TempPos++;
		if (IsEnable(TempPos, PutBit | Player))
			return true;
		TempPos++;
		if (IsEnable(TempPos, PutBit | Player))
			return true;
		TempPos += FieldWidth2;
		if (IsEnable(TempPos, PutBit | Player))
			return true;
		TempPos -= 2;
		if (IsEnable(TempPos, PutBit | Player))
			return true;
		TempPos += FieldWidth2;
		if (IsEnable(TempPos, PutBit | Player))
			return true;
		TempPos++;
		if (IsEnable(TempPos, PutBit | Player))
			return true;
		TempPos++;
		if (IsEnable(TempPos, PutBit | Player))
			return true;
		return false;
	}

	// Возвращает количество точек рядом с Pos цвета Player.
	inline const ushort NumberNearPoints(const uint Pos, const short Player)
	{
		ushort Result = 0;
		uint TempPos = Pos - FieldWidth2 - 1;
		if (IsEnable(TempPos, PutBit | Player))
			Result++;
		TempPos++;
		if (IsEnable(TempPos, PutBit | Player))
			Result++;
		TempPos++;
		if (IsEnable(TempPos, PutBit | Player))
			Result++;
		TempPos += FieldWidth2;
		if (IsEnable(TempPos, PutBit | Player))
			Result++;
		TempPos -= 2;
		if (IsEnable(TempPos, PutBit | Player))
			Result++;
		TempPos += FieldWidth2;
		if (IsEnable(TempPos, PutBit | Player))
			Result++;
		TempPos++;
		if (IsEnable(TempPos, PutBit | Player))
			Result++;
		TempPos++;
		if (IsEnable(TempPos, PutBit | Player))
			Result++;
		return Result;
	}

	// Возвращает количество групп точек рядом с CenterPos.
	inline const ushort NumberNearGroups(const uint CenterPos, const short Player)
	{
		ushort k = 0;

		if (IsNotEnable(CenterPos - 1, Player | PutBit) && (IsEnable(CenterPos - FieldWidth2 - 1, Player | PutBit) || IsEnable(CenterPos-FieldWidth2, Player | PutBit)))
			k = 1;

		if (IsNotEnable(CenterPos + FieldWidth2, Player | PutBit) && (IsEnable(CenterPos + FieldWidth2 - 1, Player | PutBit) || IsEnable(CenterPos - 1, Player | PutBit)))
			k++;

		if (IsNotEnable(CenterPos + 1, Player | PutBit) && (IsEnable(CenterPos + FieldWidth2 + 1, Player | PutBit) || IsEnable(CenterPos + FieldWidth2, Player | PutBit)))
			k++;

		if (IsNotEnable(CenterPos - FieldWidth2, Player | PutBit) && (IsEnable(CenterPos - FieldWidth2 + 1, Player | PutBit) || IsEnable(CenterPos + 1, Player | PutBit)))
			k++;

		return k;
	}

	const bool PointInsideRing(const uint TestedPos, const list<uint> &Ring)
	{
		uint Intersections = 0;

		IntersectionState State = ISNone;

		for (list<uint>::const_iterator i = Ring.begin(); i != Ring.end(); i++)
		{
			switch (GetIntersectionState(TestedPos, *i))
			{
			case (ISNone):
				State = ISNone;
				break;
			case (ISUp):
				if (State == ISDown)
					Intersections++;
				State = ISUp;
				break;
			case (ISDown):
				if (State == ISUp)
					Intersections++;
				State = ISDown;
				break;
			}
		}
		if (State == ISUp || State == ISDown)
		{
			list<uint>::const_iterator i = Ring.begin();
			IntersectionState TempState = GetIntersectionState(TestedPos, *i);
			while (TempState == State || TempState == ISTarget)
			{
				i++;
				TempState = GetIntersectionState(TestedPos, *i);
			}
			if (TempState != ISNone)
				Intersections++;
		}

		return Intersections % 2 == 1;
	}

	// Проверяет поставленную точку на наличие созданных ею окружений, и окружает, если они есть.
	void CheckClosure(const uint StartPos, short Player)
	{
		ushort InpPointsCount;
		uint InpChainPoints[4], InpSurPoints[4];

		list<uint> Chain;

		DCaptureCount = 0;
		DFreedCount = 0;

		if (IsInEmptyBase(StartPos)) // Если точка поставлена в пустую базу.
		{
			// Проверяем, в чью пустую базу поставлена точка.
			uint Pos = StartPos - 1;
			while (!IsPutted(Pos))
				Pos--;

			if (GetPlayer(Pos) == GetPlayer(StartPos)) // Если поставили в свою пустую базу.
			{
				ClearEmptyBase(StartPos);
				return;
			}

#if SURROUND_CONDITIONS
			if (SurCond != AlwaysEnemy) // Если приоритет не всегда у врага.
#endif
			{
				InpPointsCount = GetInputPoints(StartPos, Player | PutBit, InpChainPoints, InpSurPoints);
				if (InpPointsCount > 1)
				{
					for (ushort i = 0; i < InpPointsCount; i++)
						if (BuildChain(StartPos, GetPlayer(StartPos) | PutBit, InpChainPoints[i], Chain))
						{
							FindSurround(Chain, InpSurPoints[i], Player);
							// Если поставлена точка в пустую базу, и есть окружение - то это возможно только когда рядом 2 группы точек (2 точки цепочки). И только через одну из них идет окружение.
							// Если через первую - вторую проверять нет смысла. Поэтому можно смело останавливаться.
							break;
						}
					if (IsBaseBound(StartPos))
					{
						RemoveEmptyBase(StartPos);
						return;
					}
				}
			}

			Pos++;
			do
			{
				Pos--;
				while (!IsEnable(Pos, NextPlayer(Player) | PutBit))
					Pos--;
				InpPointsCount = GetInputPoints(Pos, NextPlayer(Player) | PutBit, InpChainPoints, InpSurPoints);
				for (ushort i = 0; i < InpPointsCount; i++)
					if (BuildChain(Pos, NextPlayer(Player) | PutBit, InpChainPoints[i], Chain))
						if (PointInsideRing(StartPos, Chain))
						{
							FindSurround(Chain, InpSurPoints[i], GetPlayer(Pos));
							break;
						}
			} while (!IsCaptured(StartPos));

			DCaptureCount = -1;
		}
		else
		{
			InpPointsCount = GetInputPoints(StartPos, Player | PutBit, InpChainPoints, InpSurPoints);
			if (InpPointsCount > 1)
			{
				ushort ChainsCount = 0;
				for (ushort i = 0; i < InpPointsCount; i++)
					if (BuildChain(StartPos, GetPlayer(StartPos) | PutBit, InpChainPoints[i], Chain))
					{
						FindSurround(Chain, InpSurPoints[i], Player);
						ChainsCount++;
						if (ChainsCount == InpPointsCount - 1)
							break;
					}
			}
		}
	}

	// Проверяет поставленную точку, не окружает ли она позицию CheckedPos.
	inline bool CheckClosure(const uint StartPos, const uint CheckedPos)
	{
		uint InpChainPoints[4], InpSurPoints[4];

		list<uint> Chain;

		// Цвет игрока, точка которого проверяется.
		short Player = GetPlayer(StartPos);

		ushort InpPointsCount = GetInputPoints(StartPos, Player | PutBit, InpChainPoints, InpSurPoints);
		if (InpPointsCount > 1)
		{
			for (ushort i = 0; i < InpPointsCount; i++)
				if (BuildChain(StartPos, GetPlayer(StartPos) | PutBit, InpChainPoints[i], Chain))
					if (PointInsideRing(CheckedPos, Chain))
					{
						for (list<uint>::const_iterator j = Chain.begin(); j != Chain.end(); j++)
						{
							// Добавляем в список изменений точки цепочки.
							Changes.back().Changes.push(Pair<uint, ushort>(*j, Points[*j]));
							// Помечаем точки цепочки.
							SetBaseBound(*j);
						}
						return true;
					}
		}

		return false;
	}

	inline ulong GetHash()
	{
		return Hash;
	}
};


inline int Field::Square(const uint Pos1, const uint Pos2)
{
	return (Pos1 % FieldWidth2) * (Pos2 / FieldWidth2) - (Pos1 / FieldWidth2) * (Pos2 % FieldWidth2);
}

inline const void Field::GetFirstNextPos(const uint CenterPos, uint &Pos)
{
	if (Pos < CenterPos)
	{
		if ((Pos == CenterPos - FieldWidth2 - 1) || (Pos == CenterPos - 1))
			Pos = CenterPos - FieldWidth2 + 1; 
		else
			Pos = CenterPos + FieldWidth2 + 1;
	}
	else
	{
		if ((Pos == CenterPos + 1) || (Pos == CenterPos + FieldWidth2 + 1))
			Pos = CenterPos+FieldWidth2 - 1; 
		else		
			Pos = CenterPos-FieldWidth2 - 1;
	}
}

inline const void Field::GetNextPos(const uint CenterPos, uint &Pos)
{
	uint t;
	if (Pos < CenterPos)
	{
		t = CenterPos - FieldWidth2;
		if (Pos == t - 1)
			Pos = t;
		else if (Pos == t)
			Pos = t + 1;
		else if (Pos == t + 1)
			Pos = CenterPos + 1;
		else
			Pos = t - 1;
	}
	else
	{
		t = CenterPos + FieldWidth2;
		if (Pos == CenterPos + 1)
			Pos = t + 1;
		else if (Pos == t + 1)
			Pos = t;
		else if (Pos == t)
			Pos = t - 1;
		else
			Pos = CenterPos - 1;
	}
}

inline const ushort Field::GetInputPoints(const uint CenterPos, const ushort EnableCond, uint InpChainPoints[], uint InpSurPoints[])
{
	uint k = 0;

	if (IsNotEnable(CenterPos - 1, EnableCond))
	{
		if (IsEnable(CenterPos - FieldWidth2 - 1, EnableCond))
		{
			InpChainPoints[0] = CenterPos - FieldWidth2 - 1; 
			InpSurPoints[0] = CenterPos - 1;
			k = 1;
		}
		else if (IsEnable(CenterPos-FieldWidth2, EnableCond))
		{
			InpChainPoints[0] = CenterPos - FieldWidth2; 
			InpSurPoints[0] = CenterPos - 1;
			k = 1;
		}
	}

	if (IsNotEnable(CenterPos + FieldWidth2, EnableCond))
	{
		if (IsEnable(CenterPos + FieldWidth2 - 1, EnableCond))
		{
			InpChainPoints[k] = CenterPos + FieldWidth2 - 1; 
			InpSurPoints[k] = CenterPos + FieldWidth2;
			k++;
		}
		else if (IsEnable(CenterPos - 1, EnableCond))
		{
			InpChainPoints[k] = CenterPos - 1; 
			InpSurPoints[k] = CenterPos + FieldWidth2;
			k++;
		}
	}

	if (IsNotEnable(CenterPos + 1, EnableCond))
	{
		if (IsEnable(CenterPos + FieldWidth2 + 1, EnableCond))
		{
			InpChainPoints[k] = CenterPos + FieldWidth2 + 1;
			InpSurPoints[k] = CenterPos + 1;
			k++;
		}
		else if (IsEnable(CenterPos + FieldWidth2, EnableCond)) 
		{
			InpChainPoints[k] = CenterPos + FieldWidth2; 
			InpSurPoints[k] = CenterPos + 1;
			k++;
		}
	}

	if (IsNotEnable(CenterPos - FieldWidth2, EnableCond))
	{
		if (IsEnable(CenterPos - FieldWidth2 + 1, EnableCond))
		{
			InpChainPoints[k] = CenterPos - FieldWidth2 + 1; 
			InpSurPoints[k] = CenterPos - FieldWidth2;
			k++;
		}
		else if (IsEnable(CenterPos + 1, EnableCond))
		{
			InpChainPoints[k] = CenterPos + 1; 
			InpSurPoints[k] = CenterPos - FieldWidth2;
			k++;
		}
	}

	return k;
}

inline void Field::AddSubCapturedFreed(const short Player, const int Captured, const int Freed)
{
	if (Captured == -1)
	{
		CaptureCount[NextPlayer(Player)]++;
	}
	else
	{
		CaptureCount[Player] += Captured;
		CaptureCount[NextPlayer(Player)] -= Freed;
	}
}

inline void Field::CheckCapturedAndFreed(const uint Pos, const short Player, int &Captured, int &Freed)
{
	if (IsPutted(Pos))
	{
		if (GetPlayer(Pos) != Player)
			Captured++;
		else if (IsCaptured(Pos))
			Freed++;
	}
}

inline void Field::SetCaptureFreeState(const uint Pos, const short Player)
{
	if (IsPutted(Pos))
	{
		if (GetPlayer(Pos) != Player)
			Capture(Pos);
		else
			Free(Pos);
	}
	else
		Capture(Pos);
}

inline void Field::Wave(uint StartPos, function<bool(int)> Condition, function<void(uint)> Action)
{
	// Очередь для волнового алгоритма (обхода в ширину).
	list<uint> q;
	list<uint>::const_iterator it;

	q.push_back(StartPos);
	SetTag(StartPos);
	it = q.begin();
	while (it != q.end())
	{
		Action(*it);

		if (Condition(*it - 1) && !IsTagged(*it - 1))
		{
			q.push_back(*it - 1);
			SetTag(*it - 1);
		}
		if (Condition(*it - FieldWidth2) && !IsTagged(*it - FieldWidth2))
		{
			q.push_back(*it - FieldWidth2);
			SetTag(*it - FieldWidth2);
		}
		if (Condition(*it + 1) && !IsTagged(*it + 1))
		{
			q.push_back(*it + 1);
			SetTag(*it + 1);
		}
		if (Condition(*it + FieldWidth2) && !IsTagged(*it + FieldWidth2))
		{
			q.push_back(*it + FieldWidth2);
			SetTag(*it + FieldWidth2);
		}
		it++;
	}

	for (it = q.begin(); it != q.end(); it++)
		ClearTag(*it);
}

inline void Field::UpdateHash(short Player, short Surrounded, uint Pos)
{
	Hash ^= GetZobristHash(Player, Surrounded, Pos);
}

inline const IntersectionState Field::GetIntersectionState(const uint Pos, const uint NextPos)
{
	Point a, b;
	ConvertToXY(Pos, a);
	ConvertToXY(NextPos, b);

	if (b.X <= a.X)
		switch (b.Y - a.Y)
	{
		case (1):
			return ISUp;
		case (0):
			return ISTarget;
		case (-1):
			return ISDown;
		default:
			return ISNone;
	}
	else
		return ISNone;
}