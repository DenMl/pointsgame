#pragma once
#include "Config.h"
#include "BasicTypes.h"
#include "BasicConstants.h"
#include "GameStack.h"
#include "Zobrist.h"
#include "AuxFunc.h"
#include <stdio.h>

class Field
{
#pragma region State bits and masks
	private:
	// Значение бита первого игрока.
	static const p_int PlayerRed = 0x0;
	// Значение бита второго игрока.
	static const p_int PlayerBlack = 0x1;

	// Бит, указывающий номер игрока.
	static const p_int PlayerBit = 0x1;
	// Бит, указывающий на наличие точки в поле.
	static const p_int PutBit = 0x2;
	// Бит, указывающий на захваченность точки.
	static const p_int SurBit = 0x4;
	// Бит, указывающий на то, захватывает ли что-нибудь точка в поле.
	static const p_int BoundBit = 0x8;
	// Бит, указывающий на пустую базу.
	static const p_int EmptyBaseBit = 0x20;
	// Бит для временных пометок полей.
	static const p_int TagBit = 0x80;
	// Бит, которым помечаются границы поля.
	static const p_int BadValue = 0x8;

	// Маски для определения условий.
	static const p_int EnableMask = SurBit | PutBit | PlayerBit;
	static const p_int BoundMask =  BoundBit | SurBit | PutBit | PlayerBit;
#pragma endregion

#pragma region Get and set state functions
	public:
	// Get state functions.
	// Функции получения состояния.

	// Получить по координате игрока, чья точка там поставлена.
	inline const p_int GetPlayer(const p_int Pos) { return Points[Pos] & PlayerBit; }
	// Получить по игроку следующего игрока.
	inline static p_int NextPlayer(const p_int Player) { return Player ^ PlayerBit; }
	// Проверить по координате, поставлена ли там точка.
	inline const p_int IsPutted(const p_int Pos) { return Points[Pos] & PutBit; }
	// Прверить по координате, является ли точка окружающей базу.
	inline const p_int IsBaseBound(const p_int Pos) { return Points[Pos] & BoundBit; }
	// Проверить по координате, захвачено ли поле.
	inline const p_int IsCaptured(const p_int Pos) { return Points[Pos] & SurBit; }
	// Проверить по координате, лежит ли она в пустой базе.
	inline const p_int IsInEmptyBase(const p_int Pos) { return Points[Pos] & EmptyBaseBit; }
	// Проверить по координате, помечено ли поле.
	inline const p_int IsTagged(const p_int Pos) { return Points[Pos] & TagBit; }
	// Получить условие по координате.
	inline const p_int GetEnableCond(const p_int Pos) { return Points[Pos] & EnableMask; }
	// Проверка незанятости поля по условию.
	inline const bool IsEnable(const p_int Pos, const p_int EnableCond) { return (Points[Pos] & EnableMask) == EnableCond; }
	// Проверка занятости поля по условию.
	inline const bool IsNotEnable(const p_int Pos, const p_int EnableCond) { return (Points[Pos] & EnableMask) != EnableCond; }
	// Проверка на то, захвачено ли поле.
	inline const bool IsBound(const p_int Pos, const p_int BoundCond) { return (Points[Pos] & BoundMask) == BoundCond; }
	// Проверка на то, не захвачено ли поле.
	inline const bool IsNotBound(const p_int Pos, const p_int BoundCond) { return (Points[Pos] & BoundMask) != BoundCond; }
	// Провека на то, возможно ли поставить точку в полке.
	inline const bool PuttingAllow(const p_int Pos) { return !(Points[Pos] & (PutBit | SurBit | BadValue)); }

	// Set state functions.
	// Функции установки состояния.

	// Пометить поле по координате как содержащее точку.
	inline void SetPutted(const p_int Pos) { Points[Pos] |= PutBit; }
	// Убрать с поля по координате PutBit.
	inline void ClearPutBit(const p_int Pos) { Points[Pos] &= ~PutBit; }
	// Пометить поле по координате как принадлежащее игроку.
	inline void SetPlayer(const p_int Pos, const p_int Player) { Points[Pos] = (Points[Pos] & ~PlayerBit) | Player; }
	// Пометить поле по координате как содержащее точку игрока.
	inline void SetPlayerPutted(const p_int Pos, const p_int Player) { Points[Pos] |= Player | PutBit; }
	// Пометить битом SurBit (захватить).
	inline void Capture(const p_int Pos) { Points[Pos] |= SurBit; }
	// Убрать бит SurBit.
	inline void Free(const p_int Pos) { Points[Pos] &= ~SurBit; }
	// Пометить точку как окружающую базу.
	inline void SetBaseBound(const p_int Pos) { Points[Pos] |= BoundBit; }
	// Пометить точку как не окружающую базу.
	inline void ClearBaseBound(const p_int Pos) { Points[Pos] &= ~BoundBit; }
	inline void SetInEmptyBase(const p_int Pos, const p_int Player) { Points[Pos] &= ~PlayerBit | EmptyBaseBit | Player; }
	inline void SetEmptyBase(const p_int Pos) { Points[Pos] |= EmptyBaseBit; }
	inline void ClearEmptyBase(const p_int Pos) { Points[Pos] &= ~EmptyBaseBit; }
	// Установить бит TagBit.
	inline void SetTag(const p_int Pos) { Points[Pos] |= TagBit; }
	// Убрать бит TagBit.
	inline void ClearTag(const p_int Pos) { Points[Pos] &= ~TagBit; }
#pragma endregion

#pragma region MainVariables
	public:
	// Формат одного изменения состояний (при создании базы): 
	// =================================
	// CaptureCount[0]
	// CaptureCount[1]
	// CurPlayer
	// Значение поля до изменения.
	// Координата поля.
	// ...
	// Количество изменений.
	// =================================
	// Извлечение происходит соответственно в обратном порядке
	GameStack<p_int, MAX_CHAIN_POINTS * MAX_CHAIN_POINTS> PointsChange;

	// Количество изменений доски за текущий ход.
	p_int CurrentBoardChangesNumber;

	// Main points array (game board).
	// Основной массив точек (игровая доска).
	p_int Points[PointsLength22];

	// Real field width.
	// Действительная ширина поля.
	p_int FieldWidth;
	// Real field height.
	// Действительная высота поля.
	p_int FieldHeight;

	// History points sequance.
	// Последовательность поставленных точек.
	GameStack<p_int, MAX_CHAIN_POINTS> PointsSeq;

	// Правила обработки пустых баз.
	// SurStandart = 0 - если PlayerRed ставит в пустую базу и ничего не обводит, то PlayerBlack обводит эту территорию.
	// SurAlways = 1 - обводить базу, даже если нет вражеских точек внутри.
	// SurAlwaysEnemy = 2 - обводит всегда PlayerBlack, если PlayerRed поставил точку в пустую базу.
	enum SurroundCondition
	{
		Standart,
		Always,
		AlwaysEnemy
	} SurCond;

	// Используемый шаблон в начале игры.
	// Crosswise = 0 - Crosswise.
	// Clean = 1 - Clean.
	// Square = 2 - Square.
	enum Pattern
	{
		CleanPattern,
		CrosswisePattern,
		SquarePattern
	} BeginPattern;

	// Размеры границ поля.
	p_int ExpandWidth, ExpandHeight;
	// Четность размеров поля (четные => 0; нечетные => 1).
	p_int ParityWidth, ParityHeight;

	// Current player color.
	// Текущий цвет игроков.
	enum PlayerColor
	{
		PlayerColorRed,
		PlayerColorBlack
	};
	p_int CurPlayer, EnemyPlayer;

	// Capture points count.
	// Количество захваченных точек.
	p_int CaptureCount[2];

	p_int DCaptureCount, DFreedCount;

	// Временный стек для внутренних нужд.
	GameStack<p_int, MAX_CHAIN_POINTS> TempStack;

	p_int Hash;

	public:
	// Координаты, соответствующие левому верхнему и правому нижнему углам доски.
	p_int MinPos, MaxPos;
#pragma endregion

	private:
	// Возвращает косое произведение векторов Pos1 и Pos2.
	inline static p_int Square(const p_int Pos1, const p_int Pos2)
	{
		return (Pos1 % FieldWidth2) * (Pos2 / FieldWidth2) - (Pos1 / FieldWidth2) * (Pos2 % FieldWidth2);
	}

	//  * . .   x . *   . x x   . . .
	//  . o .   x o .   . o .   . o x
	//  x x .   . . .   . . *   * . x
	//  o - center pos
	//  x - pos
	//  * - result
	inline const void GetFirstNextPos(const p_int CenterPos, p_int &Pos)
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

	//  . . .   * . .   x * .   . x *   . . x   . . .   . . .   . . .
	//  * o .   x o .   . o .   . o .   . o *   . o x   . o .   . o .
	//  x . .   . . .   . . .   . . .   . . .   . . *   . * x   * x .
	//  o - center pos
	//  x - pos
	//  * - result
	inline const void GetNextPos(const p_int CenterPos, p_int &Pos)
	{
		p_int t;
		if (Pos < CenterPos)
		{
			t = CenterPos - FieldWidth2;
			if (Pos == t - 1)
				Pos = t;
			else
				if (Pos == t)
					Pos = t + 1;
				else
					if (Pos == t + 1)
						Pos = CenterPos + 1;
					else
						Pos = t - 1;
		}
		else
		{
			t = CenterPos + FieldWidth2;
			if (Pos == CenterPos + 1)
				Pos = t + 1;
			else
				if (Pos == t + 1)
					Pos = t;
				else
					if (Pos == t)
						Pos = t - 1;
					else
						Pos = CenterPos - 1;
		}
	}

	// Возвращает количество групп точек рядом с CenterPos.
	// InpChainPoints - возможные точки цикла, InpSurPoints - возможные окруженные точки.
	inline const p_int GetInputPoints(const p_int CenterPos, const p_int EnableCond, p_int *InpChainPoints, p_int *InpSurPoints)
	{
		p_int k = 0;

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

	// Поставить начальные точки.
	inline void PlaceBeginPattern()
	{
		Point point;
		switch (BeginPattern)
		{
		case (CrosswisePattern):
			point.X = FieldWidth / 2 - 1;
			point.Y = FieldHeight / 2 - 1;
			DoStep(point);
			point.X++;
			DoStep(point);
			point.Y++;
			DoStep(point);
			point.X--;
			DoStep(point);
			break;
		case (SquarePattern):
			point.X = FieldWidth / 2 - 1;
			point.Y = FieldHeight / 2 - 1;
			DoStep(point);
			point.X++;
			DoStep(point);
			point.X--;
			point.Y++;
			DoStep(point);
			point.X++;
			DoStep(point);
			break;
		}
	}

	// Изменение счета игроков.
	inline void AddSubCapturedFreed(const p_int Player, const p_int Captured, const p_int Freed)
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

	// Изменяет Captured/Free в зависимости от того, захвачена или окружена точка.
	inline void CheckCapturedAndFreed(const p_int Pos, const p_int Player, p_int &Captured, p_int &Free)
	{
		if (IsPutted(Pos))
		{
			if (GetPlayer(Pos) != Player)
				Captured++;
			else if (IsCaptured(Pos))
				Free++;
		}
	}

	// Захватывает/освобождает окруженную точку.
	inline void SetCaptureFreeState(const p_int Pos, const p_int Player)
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

	// Заливка захваченной области.
	inline void CapturedAndFreedCount(const p_int StartPos, const p_int Player, p_int &Captured, p_int &Freed, GameStack<p_int, MAX_SUR_POINTS> &SurPoints)
	{
		SurPoints.Clear();

		Captured = 0;
		Freed = 0;
		p_int BoundCond = Player | PutBit | BoundBit;
		TempStack.Clear();
		TempStack.Push(StartPos);
		SetTag(StartPos);

		while (TempStack.Count != 0)
		{
			p_int Pos = TempStack.Pop();
			CheckCapturedAndFreed(Pos, Player, Captured, Freed);
			SurPoints.Push(Pos);

			if (IsNotBound(Pos - 1, BoundCond) && !IsTagged(Pos - 1))
			{
				TempStack.Push(Pos - 1);
				SetTag(Pos - 1);
			}

			if (IsNotBound(Pos - FieldWidth2, BoundCond) && !IsTagged(Pos - FieldWidth2))
			{
				TempStack.Push(Pos - FieldWidth2);
				SetTag(Pos - FieldWidth2);
			}

			if (IsNotBound(Pos + 1, BoundCond) && !IsTagged(Pos + 1))
			{
				TempStack.Push(Pos + 1);
				SetTag(Pos + 1);
			}

			if (IsNotBound(Pos + FieldWidth2, BoundCond) && !IsTagged(Pos + FieldWidth2))
			{
				TempStack.Push(Pos + FieldWidth2);
				SetTag(Pos + FieldWidth2);
			}
		}
	}

	// Удаляет пометку пустой базы с поля точек, начиная с позиции StartPos.
	// StartPos не заносится в список изменений поля.
	inline void RemoveEmptyBase(const p_int StartPos)
	{
		p_int Pos = StartPos;
		TempStack.Clear();
		TempStack.Push(StartPos);
		ClearEmptyBase(StartPos);

		while (TempStack.Count != 0)
		{
			Pos = TempStack.Pop();

			if (IsInEmptyBase(Pos - 1))
			{
				TempStack.Push(Pos - 1);
				CurrentBoardChangesNumber++;
				PointsChange.Push(Points[Pos - 1]);
				PointsChange.Push(Pos - 1);
				ClearEmptyBase(Pos - 1);
			}

			if (IsInEmptyBase(Pos - FieldWidth2))
			{
				TempStack.Push(Pos - FieldWidth2);
				CurrentBoardChangesNumber++;
				PointsChange.Push(Points[Pos - FieldWidth2]);
				PointsChange.Push(Pos - FieldWidth2);
				ClearEmptyBase(Pos - FieldWidth2);
			}

			if (IsInEmptyBase(Pos + 1))
			{
				TempStack.Push(Pos + 1);
				CurrentBoardChangesNumber++;
				PointsChange.Push(Points[Pos + 1]);
				PointsChange.Push(Pos + 1);
				ClearEmptyBase(Pos + 1);
			}

			if (IsInEmptyBase(Pos + FieldWidth2))
			{
				TempStack.Push(Pos + FieldWidth2);
				CurrentBoardChangesNumber++;
				PointsChange.Push(Points[Pos + FieldWidth2]);
				PointsChange.Push(Pos + FieldWidth2);
				ClearEmptyBase(Pos + FieldWidth2);
			}
		}
	}

	bool BuildChain(const p_int StartPos, const p_int InpChainPoint, GameStack<p_int, MAX_CHAIN_POINTS> &Chain)
	{
		p_int Player = GetPlayer(StartPos);

		p_int Pos;

		Chain.Clear();
		Chain.Push(StartPos);
		Pos = InpChainPoint;
		p_int CenterPos = StartPos;
		// Площадь базы.
		p_int TempSquare = Square(CenterPos, Pos);
		do
		{
			if (IsTagged(Pos))
			{
				while (Chain.Stack[Chain.Count - 1] != Pos)
				{
					Chain.Count--;
					ClearTag(Chain.Stack[Chain.Count]);
				}
			}
			else
			{
				SetTag(Pos);
				Chain.Push(Pos);
			}
			Swap(Pos, CenterPos);
			GetFirstNextPos(CenterPos, Pos);
			while (IsNotEnable(Pos, (PutBit | Player)))
				GetNextPos(CenterPos, Pos);
			TempSquare += Square(CenterPos, Pos);
		}
		while (Pos != StartPos);

		for (p_int i = 0; i < Chain.Count; i++)
			ClearTag(Chain.Stack[i]);

		return (TempSquare < 0 && Chain.Count > 2);
	}

	inline p_int BuildChains(const p_int StartPos, const p_int InpChainPoints[], const p_int InpSurPoints[], const p_int InpPointsCount, GameStack<p_int, MAX_CHAIN_POINTS> Chains[], p_int InsidePoints[])
	{
		p_int Count = 0;
		for (p_int i = 0; i < InpPointsCount; i++)
			if (BuildChain(StartPos, InpChainPoints[i], Chains[Count]))
			{
				InsidePoints[Count] = InpSurPoints[i];
				Count++;
			}

			return Count;
	}

	inline p_int BuildChainsFast(const p_int StartPos, p_int InpChainPoints[], const p_int InpSurPoints[], const p_int InpPointsCount, GameStack<p_int, MAX_CHAIN_POINTS> Chains[], p_int InsidePoints[])
	{
		p_int Count = 0;
		for (p_int i = 0; i < InpPointsCount; i++)
			if (BuildChain(StartPos, InpChainPoints[i], Chains[Count]))
			{
				InsidePoints[Count] = InpSurPoints[i];
				Count++;

				if (Count == InpPointsCount - 1)
					break;
			}

			return Count;
	}

	void FindSurround(GameStack<p_int, MAX_CHAIN_POINTS> &Chain, p_int InsidePoint, p_int Player)
	{
		// Количество захваченных точек.
		p_int CurCaptureCount = 0;
		// Количество захваченных пустых полей.
		p_int CurFreedCount = 0;

		GameStack<p_int, MAX_SUR_POINTS> SurPoints;

		// Помечаем точки цепочки.
		for (p_int i = 0; i < Chain.Count; i++)
			SetTag(Chain.Stack[i]);

		// Заливка захваченной области.
		CapturedAndFreedCount(InsidePoint, Player, CurCaptureCount, CurFreedCount, SurPoints);
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
			for (p_int i = 0; i < Chain.Count; i++)
			{
				ClearTag(Chain.Stack[i]);
				// Добавляем в список изменений точки цепочки.
				CurrentBoardChangesNumber++;
				PointsChange.Push(Points[Chain.Stack[i]]);
				PointsChange.Push(Chain.Stack[i]);
				// Помечаем точки цепочки.
				SetBaseBound(Chain.Stack[i]);
			}

			for (p_int j = 0; j < SurPoints.Count; j++)
			{
				ClearTag(SurPoints.Stack[j]);

				CurrentBoardChangesNumber++;
				PointsChange.Push(Points[SurPoints.Stack[j]]);
				PointsChange.Push(SurPoints.Stack[j]);

				SetCaptureFreeState(SurPoints.Stack[j], Player);
			}
		}
		else // Если ничего не захватили.
		{
			for (p_int i = 0; i < Chain.Count; i++)
				ClearTag(Chain.Stack[i]);

			for (p_int i = 0; i < SurPoints.Count; i++)
			{
				ClearTag(SurPoints.Stack[i]);

				CurrentBoardChangesNumber++;
				PointsChange.Push(Points[SurPoints.Stack[i]]);
				PointsChange.Push(SurPoints.Stack[i]);

				if (!IsPutted(SurPoints.Stack[i]))
					SetEmptyBase(SurPoints.Stack[i]);
			}
		}

	}

	inline void FindSurrounds(GameStack<p_int, MAX_CHAIN_POINTS> Chains[], p_int InsidePoints[], p_int ChainsCount, p_int Player)
	{
		for (p_int i = 0; i < ChainsCount; i++)
			FindSurround(Chains[i], InsidePoints[i], Player);
	}

	inline void UpdateHash(p_int Player, p_int Surrounded, p_int Pos)
	{
		Hash ^= GetZobristHash(Player, Surrounded, Pos);
	}

	enum IntersectionState
	{
		ISNone,
		ISUp,
		ISDown,
		ISTarget
	};
	const IntersectionState GetIntersectionState(const p_int Pos, const p_int NextPos)
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

public:
	// Конструктор.
	inline Field(const p_int FieldWidth, const p_int FieldHeight, const p_int SurCond, const p_int BeginPattern)
	{
		Initialize(FieldWidth, FieldHeight, SurCond, BeginPattern);
	}

	// Конструктор копирования.
	inline Field(const Field &Orig)
	{
		Copy(Orig);
	}

	// Инициализация поля.
	inline void Initialize(const p_int FieldWidth, const p_int FieldHeight, const p_int SurCond, const p_int BeginPattern)
	{
		// Очищаем массив изменений доски.
		PointsChange.Clear();
		// Очищаем количество текущих изменений.
		CurrentBoardChangesNumber = 0;

		ExpandWidth = (FieldWidth2 - FieldWidth) / 2;
		ParityWidth = (FieldWidth2 - FieldWidth) % 2;
		ExpandHeight = (FieldHeight2 - FieldHeight) / 2;
		ParityHeight = (FieldHeight2 - FieldHeight) % 2;

		// Верхнюю часть доски помечаем флагом BadValue.
		for (p_int i = 0; i < ExpandHeight * FieldWidth2; i++)
			Points[i] = BadValue;

		p_int Pos = (ExpandHeight - 1) * FieldWidth2 + ExpandWidth + FieldWidth;
		for (p_int i = 0; i < FieldHeight; i++)
		{
			// Боковые части доски помечаем флагом BadValue.
			for (p_int j = 0; j < ExpandWidth * 2 + ParityWidth; j++)
				Points[Pos++] = BadValue;
			// Очищаем центральную часть доски.
			for (p_int j = 0; j < FieldWidth; j++)
				Points[Pos++] = 0;
		}
		// Нижнюю часть доски (и нижнюю правую боковую часть) помечаем флагом BadValue.
		for (Pos; Pos < PointsLength22; Pos++)
			Points[Pos] = BadValue;

		this->FieldWidth = FieldWidth;
		this->FieldHeight = FieldHeight;
		PointsSeq.Clear();
		#if SURROUND_CONDITIONS
		this->SurCond = (SurroundCondition)SurCond;
		#endif
		this->BeginPattern = (Pattern)BeginPattern;
		CurPlayer = (PlayerColor)0x0;
		EnemyPlayer = (PlayerColor)0x1;
		CaptureCount[0] = 0;
		CaptureCount[1] = 0;

		Hash = 0;

		MinPos = UnsafeConvertToPos(0, 0);
		MaxPos = UnsafeConvertToPos(FieldWidth - 1, FieldHeight - 1);

		PlaceBeginPattern();
	}

	// Копирование доски.
	inline void Copy(const Field &Orig)
	{
		PointsChange.Copy(Orig.PointsChange);
		CurrentBoardChangesNumber = Orig.CurrentBoardChangesNumber;

		ExpandWidth = Orig.ExpandWidth;
		ParityWidth = Orig.ParityWidth;
		ExpandHeight = Orig.ExpandHeight;
		ParityHeight = Orig.ParityHeight;

		for (p_int i = 0; i < PointsLength22; i++)
			Points[i] = Orig.Points[i];

		FieldWidth = Orig.FieldWidth;
		FieldHeight = Orig.FieldHeight;
		PointsSeq.Copy(Orig.PointsSeq);
		#if SURROUND_CONDITIONS
		SurCond = Orig.SurCond;
		#endif
		BeginPattern = Orig.BeginPattern;
		CurPlayer = Orig.CurPlayer;
		EnemyPlayer = Orig.EnemyPlayer;
		CaptureCount[0] = Orig.CaptureCount[0];
		CaptureCount[1] = Orig.CaptureCount[1];

		Hash = Orig.Hash;

		MinPos = Orig.MinPos;
		MaxPos = Orig.MaxPos;
	}

	inline const p_int GetScore(p_int Player)
	{
		return CaptureCount[Player] - CaptureCount[NextPlayer(Player)];
	}

	// Функции конвертации координат.
	inline const p_int ConvertToPos(const p_int X, const p_int Y)
	{
		if ((X < 0) || (X >= FieldWidth) || (Y < 0) || (Y >= FieldHeight))
			return 0;
		else
			return (Y + ExpandHeight) * FieldWidth2 + X + ExpandWidth;
	}
	inline const p_int ConvertToPos(const Point point)
	{
		return ConvertToPos(point.X, point.Y);
	}
	// Небезопасные функции конвертации координат.
	inline const p_int UnsafeConvertToPos(const p_int X, const p_int Y)
	{
		return (Y + ExpandHeight) * FieldWidth2 + X + ExpandWidth;
	}
	inline const p_int UnsafeConvertToPos(const Point point)
	{
		return UnsafeConvertToPos(point.X, point.Y);
	}
	// Конвертация из Pos в XY.
	inline const void ConvertToXY(const p_int Pos, p_int &X, p_int &Y)
	{
		X = Pos % FieldWidth2 - ExpandWidth;
		Y = Pos / FieldWidth2 - ExpandHeight;
	}
	inline const void ConvertToXY(const p_int Pos, Point &P)
	{
		P.X = Pos % FieldWidth2 - ExpandWidth;
		P.Y = Pos / FieldWidth2 - ExpandHeight;
	}

	// Поставить точку на поле следующего по очереди игрока.
	inline bool DoStep(const p_int Pos)
	{
		if (PuttingAllow(Pos))
		{
			DoUnsafeStep(Pos);
			return true;
		}
		return false;
	}
	inline bool DoStep(const Point point)
	{
		return DoStep(ConvertToPos(point));
	}
	// Поставить точку на поле.
	inline bool DoStep(const p_int Pos, const p_int Player)
	{
		if (PuttingAllow(Pos))
		{
			DoUnsafeStep(Pos, Player);
			return true;
		}
		return false;
	}
	inline bool DoStep(const Point point, const p_int Player)
	{
		return DoStep(ConvertToPos(point), Player);
	}
	// Поставить точку на поле максимально быстро (без дополнительных проверок).
	inline void DoUnsafeStep(const p_int Pos)
	{
		PointsChange.Push(CaptureCount[0]);
		PointsChange.Push(CaptureCount[1]);
		PointsChange.Push(CurPlayer);

		// Сбрасываем текущее количество изменений доски и добавляем в изменения поставленную точку.
		CurrentBoardChangesNumber = 1;
		PointsChange.Push(Points[Pos]);
		PointsChange.Push(Pos);

		SetPlayer(Pos, CurPlayer);
		SetPutted(Pos);

		PointsSeq.Push(Pos);

		SetNextPlayer();

		CheckClosure(Pos);

		PointsChange.Push(CurrentBoardChangesNumber);
	}
	inline void DoUnsafeStep(const Point point)
	{
		DoStep(UnsafeConvertToPos(point));
	}
	// Поставить точку на поле следующего по очереди игрока максимально быстро (без дополнительных проверок).
	inline void DoUnsafeStep(const p_int Pos, const p_int Player)
	{
		PointsChange.Push(CaptureCount[0]);
		PointsChange.Push(CaptureCount[1]);
		PointsChange.Push(CurPlayer);

		// Сбрасываем текущее количество изменений доски и добавляем в изменения поставленную точку.
		CurrentBoardChangesNumber = 1;
		PointsChange.Push(Points[Pos]);
		PointsChange.Push(Pos);

		SetPlayer(Pos, Player);
		SetPutted(Pos);

		PointsSeq.Push(Pos);

		CheckClosure(Pos);

		PointsChange.Push(CurrentBoardChangesNumber);
	}
	inline void DoUnsafeStep(const Point point, const p_int Player)
	{
		DoStep(UnsafeConvertToPos(point), Player);
	}

	// Делает ход и проверяет на окруженность только точку CheckedPos.
	inline bool DoUnsafeStepAndCheckPoint(const p_int Pos, const p_int Player, const p_int CheckedPos)
	{
		PointsChange.Push(CaptureCount[0]);
		PointsChange.Push(CaptureCount[1]);
		PointsChange.Push(CurPlayer);

		// Сбрасываем текущее количество изменений доски и добавляем в изменения поставленную точку.
		CurrentBoardChangesNumber = 1;
		PointsChange.Push(Points[Pos]);
		PointsChange.Push(Pos);

		SetPlayer(Pos, Player);
		SetPutted(Pos);

		PointsSeq.Push(Pos);
		
		bool result = CheckClosure(Pos, CheckedPos);

		PointsChange.Push(CurrentBoardChangesNumber);

		return result;
	}

	// Откат хода.
	inline void UndoStep()
	{
		PointsSeq.Count--;
		p_int ChangeNumber = PointsChange.Pop();
		p_int temp;
		for (p_int i = 0; i < ChangeNumber; i++)
		{
			temp = PointsChange.Pop();
			Points[temp] = PointsChange.Pop();
		}
		CurPlayer = PointsChange.Pop();
		EnemyPlayer = NextPlayer(CurPlayer);
		CaptureCount[1] = PointsChange.Pop();
		CaptureCount[0] = PointsChange.Pop();
	}

	// Установить следующего игрока как текущего.
	inline void SetNextPlayer()
	{
		CurPlayer ^= PlayerBit;
		EnemyPlayer ^= PlayerBit;
	}

	inline void SetCurrentPlayer(const p_int Player)
	{
		CurPlayer = Player;
		EnemyPlayer = NextPlayer(Player);
	}

	// Проверяет, находятся ли две точки рядом.
	inline static bool IsNear(const p_int Pos1, const p_int Pos2)
	{
		p_int TempPos = Pos1 - FieldWidth2 - 1;
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
	inline const bool IsNearPoints(const p_int Pos, const p_int Player)
	{
		p_int TempPos = Pos - FieldWidth2 - 1;
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
	inline const p_int NumberNearPoints(const p_int Pos, const p_int Player)
	{
		p_int Result = 0;
		p_int TempPos = Pos - FieldWidth2 - 1;
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
	inline const p_int NumberNearGroups(const p_int CenterPos, const p_int Player)
	{
		p_int k = 0;

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

	const bool PointInsideRing(const p_int TestedPos, const GameStack<p_int, MAX_CHAIN_POINTS> &Ring)
	{
		p_int Intersections = 0;

		IntersectionState State = ISNone;

		for (p_int i = 0; i < Ring.Count; i++)
		{
			switch (GetIntersectionState(TestedPos, Ring.Stack[i]))
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
			IntersectionState TempState = GetIntersectionState(TestedPos, Ring.Stack[0]);
			p_int i = 0;
			while (TempState == State || TempState == ISTarget)
			{
				i++;
				TempState = GetIntersectionState(TestedPos, Ring.Stack[i]);
			}
			if (TempState != ISNone)
				Intersections++;
		}

		return Intersections % 2 == 1;
	}

	// Проверяет поставленную точку на наличие созданных ею окружений, и окружает, если они есть.
	inline void CheckClosure(const p_int StartPos)
	{
		p_int InpPointsCount;
		p_int InpChainPoints[4], InpSurPoints[4];

		GameStack<p_int, MAX_CHAIN_POINTS> Chains[4];
		p_int InsidePoints[4];
		p_int ChainsCount;

		DCaptureCount = 0;
		DFreedCount = 0;

		// Цвет игрока, точка которого проверяется.
		p_int OutPlayer = GetPlayer(StartPos);

		if (IsInEmptyBase(StartPos)) // Если точка поставлена в пустую базу.
		{
			// Проверяем, в чью пустую базу поставлена точка.
			p_int Pos = StartPos - 1;
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
				InpPointsCount = GetInputPoints(StartPos, OutPlayer | PutBit, InpChainPoints, InpSurPoints);
				if (InpPointsCount > 1)
				{
					ChainsCount = BuildChainsFast(StartPos, InpChainPoints, InpSurPoints, InpPointsCount, Chains, InsidePoints);
					FindSurrounds(Chains, InsidePoints, ChainsCount, OutPlayer);
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
				do
				{
					Pos--;
					while (!IsEnable(Pos, NextPlayer(OutPlayer) | PutBit))
						Pos--;
					InpPointsCount = GetInputPoints(Pos, NextPlayer(OutPlayer) | PutBit, InpChainPoints, InpSurPoints);
					ChainsCount = BuildChains(Pos, InpChainPoints, InpSurPoints, InpPointsCount, Chains, InsidePoints);
				} while (ChainsCount == 0);

				for (p_int i = 0; i < ChainsCount; i++)
					if (PointInsideRing(StartPos, Chains[i]))
						FindSurround(Chains[i], InsidePoints[i], GetPlayer(Pos));
			} while (!IsCaptured(StartPos));

			DCaptureCount = -1;
			return;
		}

		InpPointsCount = GetInputPoints(StartPos, OutPlayer | PutBit, InpChainPoints, InpSurPoints);
		if (InpPointsCount > 1)
		{
			ChainsCount = BuildChainsFast(StartPos, InpChainPoints, InpSurPoints, InpPointsCount, Chains, InsidePoints);
			FindSurrounds(Chains, InsidePoints, ChainsCount, OutPlayer);
		}
	}

	// Проверяет поставленную точку, не окружает ли она позицию CheckedPos.
	inline bool CheckClosure(const p_int StartPos, const p_int CheckedPos)
	{
		p_int InpChainPoints[4], InpSurPoints[4];

		GameStack<p_int, MAX_CHAIN_POINTS> Chains[4];
		p_int InsidePoints[4];

		// Цвет игрока, точка которого проверяется.
		p_int OutPlayer = GetPlayer(StartPos);

		p_int InpPointsCount = GetInputPoints(StartPos, OutPlayer | PutBit, InpChainPoints, InpSurPoints);
		if (InpPointsCount > 1)
		{
			p_int ChainsCount = BuildChainsFast(StartPos, InpChainPoints, InpSurPoints, InpPointsCount, Chains, InsidePoints);
			for (p_int i = 0; i < ChainsCount; i++)
				if (PointInsideRing(CheckedPos, Chains[i]))
				{
					for (p_int j = 0; j < Chains[i].Count; j++)
					{
						// Добавляем в список изменений точки цепочки.
						CurrentBoardChangesNumber++;
						PointsChange.Push(Points[Chains[i].Stack[j]]);
						PointsChange.Push(Chains[i].Stack[j]);
						// Помечаем точки цепочки.
						SetBaseBound(Chains[i].Stack[j]);
					}
					return true;
				}
		}

		return false;
	}

	inline p_int GetHash()
	{
		return Hash;
	}

	#if DEBUG
	public:
	// Отладочная доска.
	p_int DebugPoints[PointsLength22];
	// Функция копирования доски в отладучную для последующих сравнений.
	void CopyToDebugPoints()
	{
		for (p_int i = 0; i < PointsLength22; i++)
			DebugPoints[i] = Points[i];
	}
	// Функция сравнения доски с отладочной доской.
	bool CompareWithDebugPoints()
	{
		for (p_int i = 0; i < PointsLength22; i++)
			if (DebugPoints[i] != Points[i])
				return false;
		return true;
	}

	void Save()
	{
		FILE *f = fopen("debug.txt", "w");
		p_int x, y;
		fprintf(f, "(SZX[%d] SZY[%d]\n", FieldWidth, FieldHeight);
		for (p_int i = 0; i < PointsSeq.Count; i++)
		{
			ConvertToXY(PointsSeq.Stack[i], x, y);
			if (GetPlayer(PointsSeq.Stack[i]) == 0)
				fprintf(f, ";B");
			else
				fprintf(f, ";R");
			fprintf(f, "[%02d-%02d]\n", x, y);
		}
		fprintf(f, ")");
		fclose(f);
	}
	#endif
};