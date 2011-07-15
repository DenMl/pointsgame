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
	// �������� ���� ������� ������.
	static const _int PlayerRed = 0x0;
	// �������� ���� ������� ������.
	static const _int PlayerBlack = 0x1;

	// ���, ����������� ����� ������.
	static const _int PlayerBit = 0x1;
	// ���, ����������� �� ������� ����� � ����.
	static const _int PutBit = 0x2;
	// ���, ����������� �� ������������� �����.
	static const _int SurBit = 0x4;
	// ���, ����������� �� ��, ����������� �� ���-������ ����� � ����.
	static const _int BoundBit = 0x8;
	// ���, ����������� �� ������ ����.
	static const _int EmptyBaseBit = 0x20;
	// ��� ��� ��������� ������� �����.
	static const _int TagBit = 0x80;
	// ���, ������� ���������� ������� ����.
	static const _int BadValue = 0x8;

	// ����� ��� ����������� �������.
	static const _int EnableMask = SurBit | PutBit | PlayerBit;
	static const _int BoundMask =  BoundBit | SurBit | PutBit | PlayerBit;
#pragma endregion

#pragma region Get and set state functions
	public:
	// Get state functions.
	// ������� ��������� ���������.

	// �������� �� ���������� ������, ��� ����� ��� ����������.
	inline const _int GetPlayer(const _int Pos) { return Points[Pos] & PlayerBit; }
	// �������� �� ������ ���������� ������.
	inline static _int NextPlayer(const _int Player) { return Player ^ PlayerBit; }
	// ��������� �� ����������, ���������� �� ��� �����.
	inline const _int IsPutted(const _int Pos) { return Points[Pos] & PutBit; }
	// �������� �� ����������, �������� �� ����� ���������� ����.
	inline const _int IsBaseBound(const _int Pos) { return Points[Pos] & BoundBit; }
	// ��������� �� ����������, ��������� �� ����.
	inline const _int IsCaptured(const _int Pos) { return Points[Pos] & SurBit; }
	// ��������� �� ����������, ����� �� ��� � ������ ����.
	inline const _int IsInEmptyBase(const _int Pos) { return Points[Pos] & EmptyBaseBit; }
	// ��������� �� ����������, �������� �� ����.
	inline const _int IsTagged(const _int Pos) { return Points[Pos] & TagBit; }
	// �������� ������� �� ����������.
	inline const _int GetEnableCond(const _int Pos) { return Points[Pos] & EnableMask; }
	// �������� ����������� ���� �� �������.
	inline const bool IsEnable(const _int Pos, const _int EnableCond) { return (Points[Pos] & EnableMask) == EnableCond; }
	// �������� ��������� ���� �� �������.
	inline const bool IsNotEnable(const _int Pos, const _int EnableCond) { return (Points[Pos] & EnableMask) != EnableCond; }
	// �������� �� ��, ��������� �� ����.
	inline const bool IsBound(const _int Pos, const _int BoundCond) { return (Points[Pos] & BoundMask) == BoundCond; }
	// �������� �� ��, �� ��������� �� ����.
	inline const bool IsNotBound(const _int Pos, const _int BoundCond) { return (Points[Pos] & BoundMask) != BoundCond; }
	// ������� �� ��,�������� �� ��������� ����� � �����.
	inline const bool PuttingAllow(const _int Pos) { return !(Points[Pos] & (PutBit | SurBit | BadValue)); }

	// Set state functions.
	// ������� ��������� ���������.

	// �������� ���� �� ���������� ��� ���������� �����.
	inline void SetPutted(const _int Pos) { Points[Pos] |= PutBit; }
	// ������ � ���� �� ���������� PutBit.
	inline void ClearPutBit(const _int Pos) { Points[Pos] &= ~PutBit; }
	// �������� ���� �� ���������� ��� ������������� ������.
	inline void SetPlayer(const _int Pos, const _int Player) { Points[Pos] = (Points[Pos] & ~PlayerBit) | Player; }
	// �������� ���� �� ���������� ��� ���������� ����� ������.
	inline void SetPlayerPutted(const _int Pos, const _int Player) { Points[Pos] |= Player | PutBit; }
	// �������� ����� SurBit (���������).
	inline void Capture(const _int Pos) { Points[Pos] |= SurBit; }
	// ������ ��� SurBit.
	inline void Free(const _int Pos) { Points[Pos] &= ~SurBit; }
	// �������� ����� ��� ���������� ����.
	inline void SetBaseBound(const _int Pos) { Points[Pos] |= BoundBit; }
	// �������� ����� ��� �� ���������� ����.
	inline void ClearBaseBound(const _int Pos) { Points[Pos] &= ~BoundBit; }
	inline void SetInEmptyBase(const _int Pos, const _int Player) { Points[Pos] &= ~PlayerBit | EmptyBaseBit | Player; }
	inline void SetEmptyBase(const _int Pos) { Points[Pos] |= EmptyBaseBit; }
	inline void ClearEmptyBase(const _int Pos) { Points[Pos] &= ~EmptyBaseBit; }
	// ���������� ��� TagBit.
	inline void SetTag(const _int Pos) { Points[Pos] |= TagBit; }
	// ������ ��� TagBit.
	inline void ClearTag(const _int Pos) { Points[Pos] &= ~TagBit; }
#pragma endregion

#pragma region MainVariables
	public:
	// ������ ������ ��������� ��������� (��� �������� ����): 
	// =================================
	// CaptureCount[0]
	// CaptureCount[1]
	// CurPlayer
	// �������� ���� �� ���������.
	// ���������� ����.
	// ...
	// ���������� ���������.
	// =================================
	// ���������� ���������� �������������� � �������� �������
	GameStack<_int, MAX_CHAIN_POINTS * MAX_CHAIN_POINTS> PointsChange;

	// ���������� ��������� ����� �� ������� ���.
	_int CurrentBoardChangesNumber;

	// Main points array (game board).
	// �������� ������ ����� (������� �����).
	_int Points[PointsLength22];

	// Real field width.
	// �������������� ������ ����.
	_int FieldWidth;
	// Real field height.
	// �������������� ������ ����.
	_int FieldHeight;

	// History points sequance.
	// ������������������ ������������ �����.
	GameStack<_int, MAX_CHAIN_POINTS> PointsSeq;

	// ������� ��������� ������ ���.
	// SurStandart = 0 - ���� PlayerRed ������ � ������ ���� � ������ �� �������, �� PlayerBlack ������� ��� ����������.
	// SurAlways = 1 - �������� ����, ���� ���� ��� ��������� ����� ������.
	// SurAlwaysEnemy = 2 - ������� ������ PlayerBlack, ���� PlayerRed �������� ����� � ������ ����.
	enum SurroundCondition
	{
		Standart,
		Always,
		AlwaysEnemy
	} SurCond;

	// ������������ ������ � ������ ����.
	// Crosswise = 0 - Crosswise.
	// Clean = 1 - Clean.
	// Square = 2 - Square.
	enum Pattern
	{
		CleanPattern,
		CrosswisePattern,
		SquarePattern
	} BeginPattern;

	// ������� ������ ����.
	_int ExpandWidth, ExpandHeight;
	// �������� �������� ���� (������ => 0; �������� => 1).
	_int ParityWidth, ParityHeight;

	// Current player color.
	// ������� ���� �������.
	enum PlayerColor
	{
		PlayerColorRed,
		PlayerColorBlack
	};
	_int CurPlayer, EnemyPlayer;

	// Capture points count.
	// ���������� ����������� �����.
	_int CaptureCount[2];

	_int DCaptureCount, DFreedCount;

	// ��������� ���� ��� ���������� ����.
	GameStack<_int, MAX_CHAIN_POINTS> TempStack;

	_int Hash;

	public:
	// ����������, ��������������� ������ �������� � ������� ������� ����� �����.
	_int MinPos, MaxPos;
#pragma endregion

	private:
	// ���������� ����� ������������ �������� Pos1 � Pos2.
	inline static _int Square(const _int Pos1, const _int Pos2)
	{
		return (Pos1 % FieldWidth2) * (Pos2 / FieldWidth2) - (Pos1 / FieldWidth2) * (Pos2 % FieldWidth2);
	}

	//  * . .   x . *   . x x   . . .
	//  . o .   x o .   . o .   . o x
	//  x x .   . . .   . . *   * . x
	//  o - center pos
	//  x - pos
	//  * - result
	inline const void GetFirstNextPos(const _int CenterPos, _int &Pos)
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
	inline const void GetNextPos(const _int CenterPos, _int &Pos)
	{
		_int t;
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

	// ���������� ���������� ����� ����� ����� � CenterPos.
	// InpChainPoints - ��������� ����� �����, InpSurPoints - ��������� ���������� �����.
	inline const _int GetInputPoints(const _int CenterPos, const _int EnableCond, _int *InpChainPoints, _int *InpSurPoints)
	{
		_int k = 0;

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

	// ��������� ��������� �����.
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

	// ��������� ����� �������.
	inline void AddSubCapturedFreed(const _int Player, const _int Captured, const _int Freed)
	{
		if (Captured == MinusOne)
		{
			CaptureCount[NextPlayer(Player)]++;
		}
		else
		{
			CaptureCount[Player] += Captured;
			CaptureCount[NextPlayer(Player)] -= Freed;
		}
	}

	// �������� Captured/Free � ����������� �� ����, ��������� ��� �������� �����.
	inline void CheckCapturedAndFreed(const _int Pos, const _int Player, _int &Captured, _int &Free)
	{
		if (IsPutted(Pos))
		{
			if (GetPlayer(Pos) != Player)
				Captured++;
			else if (IsCaptured(Pos))
				Free++;
		}
	}

	// �����������/����������� ���������� �����.
	inline void SetCaptureFreeState(const _int Pos, const _int Player)
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

	// ������� ����������� �������.
	inline void CapturedAndFreedCount(const _int StartPos, const _int Player, _int &Captured, _int &Freed, GameStack<_int, MAX_SUR_POINTS> &SurPoints)
	{
		SurPoints.Clear();

		Captured = 0;
		Freed = 0;
		_int BoundCond = Player | PutBit | BoundBit;
		TempStack.Clear();
		TempStack.Push(StartPos);
		SetTag(StartPos);

		while (TempStack.Count != 0)
		{
			_int Pos = TempStack.Pop();
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

	// ������� ������� ������ ���� � ���� �����, ������� � ������� StartPos.
	// StartPos �� ��������� � ������ ��������� ����.
	inline void RemoveEmptyBase(const _int StartPos)
	{
		_int Pos = StartPos;
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

	bool BuildChain(const _int StartPos, const _int InpChainPoint, GameStack<_int, MAX_CHAIN_POINTS> &Chain)
	{
		_int Player = GetPlayer(StartPos);

		_int Pos;

		Chain.Clear();
		Chain.Push(StartPos);
		Pos = InpChainPoint;
		_int CenterPos = StartPos;
		// ������� ����.
		_int TempSquare = Square(CenterPos, Pos);
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

		for (_int i = 0; i < Chain.Count; i++)
			ClearTag(Chain.Stack[i]);

		return (TempSquare < 0 && Chain.Count > 2);
	}

	inline _int BuildChains(const _int StartPos, const _int InpChainPoints[], const _int InpSurPoints[], const _int InpPointsCount, GameStack<_int, MAX_CHAIN_POINTS> Chains[], _int InsidePoints[])
	{
		_int Count = 0;
		for (_int i = 0; i < InpPointsCount; i++)
			if (BuildChain(StartPos, InpChainPoints[i], Chains[Count]))
			{
				InsidePoints[Count] = InpSurPoints[i];
				Count++;
			}

			return Count;
	}

	inline _int BuildChainsFast(const _int StartPos, _int InpChainPoints[], const _int InpSurPoints[], const _int InpPointsCount, GameStack<_int, MAX_CHAIN_POINTS> Chains[], _int InsidePoints[])
	{
		_int Count = 0;
		for (_int i = 0; i < InpPointsCount; i++)
			if (BuildChain(StartPos, InpChainPoints[i], Chains[Count]))
			{
				InsidePoints[Count] = InpSurPoints[i];
				Count++;

				if (Count == InpPointsCount - 1)
					break;
			}

			return Count;
	}

	void FindSurround(GameStack<_int, MAX_CHAIN_POINTS> &Chain, _int InsidePoint, _int Player)
	{
		// ���������� ����������� �����.
		_int CurCaptureCount = 0;
		// ���������� ����������� ������ �����.
		_int CurFreedCount = 0;

		GameStack<_int, MAX_SUR_POINTS> SurPoints;

		// �������� ����� �������.
		for (_int i = 0; i < Chain.Count; i++)
			SetTag(Chain.Stack[i]);

		// ������� ����������� �������.
		CapturedAndFreedCount(InsidePoint, Player, CurCaptureCount, CurFreedCount, SurPoints);
		// ��������� ����� �������.
		AddSubCapturedFreed(Player, CurCaptureCount, CurFreedCount);
		DCaptureCount += CurCaptureCount;
		DFreedCount += CurFreedCount;

		#if SURROUND_CONDITIONS
		if ((CurCaptureCount != 0) || (SurCond == Always)) // ���� ��������� �����, ��� ����� ����� ����������� ������.
		#else
		if (CurCaptureCount != 0) // ���� ��������� �����.
		#endif
		{
			for (_int i = 0; i < Chain.Count; i++)
			{
				ClearTag(Chain.Stack[i]);
				// ��������� � ������ ��������� ����� �������.
				CurrentBoardChangesNumber++;
				PointsChange.Push(Points[Chain.Stack[i]]);
				PointsChange.Push(Chain.Stack[i]);
				// �������� ����� �������.
				SetBaseBound(Chain.Stack[i]);
			}

			for (_int j = 0; j < SurPoints.Count; j++)
			{
				ClearTag(SurPoints.Stack[j]);

				CurrentBoardChangesNumber++;
				PointsChange.Push(Points[SurPoints.Stack[j]]);
				PointsChange.Push(SurPoints.Stack[j]);

				SetCaptureFreeState(SurPoints.Stack[j], Player);
			}
		}
		else // ���� ������ �� ���������.
		{
			for (_int i = 0; i < Chain.Count; i++)
				ClearTag(Chain.Stack[i]);

			for (_int i = 0; i < SurPoints.Count; i++)
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

	inline void FindSurrounds(GameStack<_int, MAX_CHAIN_POINTS> Chains[], _int InsidePoints[], _int ChainsCount, _int Player)
	{
		for (_int i = 0; i < ChainsCount; i++)
			FindSurround(Chains[i], InsidePoints[i], Player);
	}

	inline void UpdateHash(_int Player, _int Surrounded, _int Pos)
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
	const IntersectionState GetIntersectionState(const _int Pos, const _int NextPos)
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
	// �����������.
	inline Field(const _int FieldWidth, const _int FieldHeight, const _int SurCond, const _int BeginPattern)
	{
		Initialize(FieldWidth, FieldHeight, SurCond, BeginPattern);
	}

	// ����������� �����������.
	inline Field(const Field &Orig)
	{
		Copy(Orig);
	}

	// ������������� ����.
	inline void Initialize(const _int FieldWidth, const _int FieldHeight, const _int SurCond, const _int BeginPattern)
	{
		// ������� ������ ��������� �����.
		PointsChange.Clear();
		// ������� ���������� ������� ���������.
		CurrentBoardChangesNumber = 0;

		ExpandWidth = (FieldWidth2 - FieldWidth) / 2;
		ParityWidth = (FieldWidth2 - FieldWidth) % 2;
		ExpandHeight = (FieldHeight2 - FieldHeight) / 2;
		ParityHeight = (FieldHeight2 - FieldHeight) % 2;

		MinPos = ExpandHeight * FieldWidth2 + 1;
		MaxPos = (ExpandHeight + FieldHeight) * FieldWidth2;

		// ������� ����� ����� �������� ������ BadValue.
		for (_int i = 0; i < ExpandHeight * FieldWidth2; i++)
			Points[i] = BadValue;

		_int Pos = (ExpandHeight - 1) * FieldWidth2 + ExpandWidth + FieldWidth;
		for (_int i = 0; i < FieldHeight; i++)
		{
			// ������� ����� ����� �������� ������ BadValue.
			for (_int j = 0; j < ExpandWidth * 2 + ParityWidth; j++)
				Points[Pos++] = BadValue;
			// ������� ����������� ����� �����.
			for (_int j = 0; j < FieldWidth; j++)
				Points[Pos++] = 0;
		}
		// ������ ����� ����� (� ������ ������ ������� �����) �������� ������ BadValue.
		for (Pos; Pos < PointsLength22; Pos++)
			Points[Pos] = BadValue;

		//PlaceBeginPattern();

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

		PlaceBeginPattern();
	}

	// ����������� �����.
	inline void Copy(const Field &Orig)
	{
		PointsChange.Copy(Orig.PointsChange);
		CurrentBoardChangesNumber = Orig.CurrentBoardChangesNumber;

		ExpandWidth = Orig.ExpandWidth;
		ParityWidth = Orig.ParityWidth;
		ExpandHeight = Orig.ExpandHeight;
		ParityHeight = Orig.ParityHeight;

		MinPos = Orig.MinPos;
		MaxPos = Orig.MaxPos;

		for (_int i = 0; i < PointsLength22; i++)
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
	}

	// ������� ����������� ���������.
	inline const _int ConvertToPos(const _int X, const _int Y)
	{
		if ((X < 0) || (X >= FieldWidth) || (Y < 0) || (Y >= FieldHeight))
			return 0;
		else
			return ((Y + ExpandHeight) * FieldWidth2 + X + ExpandWidth);
	}
	inline const _int ConvertToPos(const Point point)
	{
		return ConvertToPos(point.X, point.Y);
	}
	// ������������ ������� ����������� ���������.
	inline const _int UnsafeConvertToPos(const _int X, const _int Y)
	{
		return ((Y + ExpandHeight) * FieldWidth2 + X + ExpandWidth);
	}
	inline const _int UnsafeConvertToPos(const Point point)
	{
		return UnsafeConvertToPos(point.X, point.Y);
	}
	// ����������� �� Pos � XY.
	inline const void ConvertToXY(const _int Pos, _int &X, _int &Y)
	{
		X = Pos % FieldWidth2 - ExpandWidth;
		Y = Pos / FieldWidth2 - ExpandHeight;
	}
	inline const void ConvertToXY(const _int Pos, Point &P)
	{
		P.X = Pos % FieldWidth2 - ExpandWidth;
		P.Y = Pos / FieldWidth2 - ExpandHeight;
	}

	// ��������� ����� �� ���� ���������� �� ������� ������.
	inline bool DoStep(const _int Pos)
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
	// ��������� ����� �� ����.
	inline bool DoStep(const _int Pos, const _int Player)
	{
		if (PuttingAllow(Pos))
		{
			DoUnsafeStep(Pos, Player);
			return true;
		}
		return false;
	}
	inline bool DoStep(const Point point, const _int Player)
	{
		return DoStep(ConvertToPos(point), Player);
	}
	// ��������� ����� �� ���� ����������� ������ (��� �������������� ��������).
	inline void DoUnsafeStep(const _int Pos)
	{
		PointsChange.Push(CaptureCount[0]);
		PointsChange.Push(CaptureCount[1]);
		PointsChange.Push(CurPlayer);

		// ���������� ������� ���������� ��������� ����� � ��������� � ��������� ������������ �����.
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
	// ��������� ����� �� ���� ���������� �� ������� ������ ����������� ������ (��� �������������� ��������).
	inline void DoUnsafeStep(const _int Pos, const _int Player)
	{
		PointsChange.Push(CaptureCount[0]);
		PointsChange.Push(CaptureCount[1]);
		PointsChange.Push(CurPlayer);

		// ���������� ������� ���������� ��������� ����� � ��������� � ��������� ������������ �����.
		CurrentBoardChangesNumber = 1;
		PointsChange.Push(Points[Pos]);
		PointsChange.Push(Pos);

		SetPlayer(Pos, Player);
		SetPutted(Pos);

		PointsSeq.Push(Pos);

		CheckClosure(Pos);

		PointsChange.Push(CurrentBoardChangesNumber);
	}
	inline void DoUnsafeStep(const Point point, const _int Player)
	{
		DoStep(UnsafeConvertToPos(point), Player);
	}

	// ������ ��� � ��������� �� ������������ ������ ����� CheckedPos.
	inline bool DoUnsafeStepAndCheckPoint(const _int Pos, const _int Player, const _int CheckedPos)
	{
		PointsChange.Push(CaptureCount[0]);
		PointsChange.Push(CaptureCount[1]);
		PointsChange.Push(CurPlayer);

		// ���������� ������� ���������� ��������� ����� � ��������� � ��������� ������������ �����.
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

	// ����� ����.
	inline void UndoStep()
	{
		PointsSeq.Count--;
		_int ChangeNumber = PointsChange.Pop();
		_int temp;
		for (_int i = 0; i < ChangeNumber; i++)
		{
			temp = PointsChange.Pop();
			Points[temp] = PointsChange.Pop();
		}
		CurPlayer = PointsChange.Pop();
		EnemyPlayer = NextPlayer(CurPlayer);
		CaptureCount[1] = PointsChange.Pop();
		CaptureCount[0] = PointsChange.Pop();
	}

	// ���������� ���������� ������ ��� ��������.
	inline void SetNextPlayer()
	{
		CurPlayer ^= PlayerBit;
		EnemyPlayer ^= PlayerBit;
	}

	inline void SetCurrentPlayer(const _int Player)
	{
		CurPlayer = Player;
		EnemyPlayer = NextPlayer(Player);
	}

	// ���������, ��������� �� ��� ����� �����.
	inline static bool IsNear(const _int Pos1, const _int Pos2)
	{
		_int TempPos = Pos1 - FieldWidth2 - 1;
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

	// ���������, ���� �� ����� � Pos ����� ����� Player.
	inline const bool IsNearPoints(const _int Pos, const _int Player)
	{
		_int TempPos = Pos - FieldWidth2 - 1;
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

	// ���������� ���������� ����� ����� � Pos ����� Player.
	inline const _int NumberNearPoints(const _int Pos, const _int Player)
	{
		_int Result = 0;
		_int TempPos = Pos - FieldWidth2 - 1;
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

	// ���������� ���������� ����� ����� ����� � CenterPos.
	inline const _int NumberNearGroups(const _int CenterPos, const _int Player)
	{
		_int k = 0;

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

	const bool PointInsideRing(const _int TestedPos, const GameStack<_int, MAX_CHAIN_POINTS> &Ring)
	{
		_int Intersections = 0;

		IntersectionState State = ISNone;

		for (_int i = 0; i < Ring.Count; i++)
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
			_int i = 0;
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

	// ��������� ������������ ����� �� ������� ��������� �� ���������, � ��������, ���� ��� ����.
	inline void CheckClosure(const _int StartPos)
	{
		_int InpPointsCount;
		_int InpChainPoints[4], InpSurPoints[4];

		GameStack<_int, MAX_CHAIN_POINTS> Chains[4];
		_int InsidePoints[4];
		_int ChainsCount;

		DCaptureCount = 0;
		DFreedCount = 0;

		// ���� ������, ����� �������� �����������.
		_int OutPlayer = GetPlayer(StartPos);

		if (IsInEmptyBase(StartPos)) // ���� ����� ���������� � ������ ����.
		{
			// ���������, � ��� ������ ���� ���������� �����.
			_int Pos = StartPos - 1;
			while (!IsPutted(Pos))
				Pos--;

			if (GetPlayer(Pos) == GetPlayer(StartPos)) // ���� ��������� � ���� ������ ����.
			{
				ClearEmptyBase(StartPos);
				return;
			}

			#if SURROUND_CONDITIONS
			if (SurCond != AlwaysEnemy) // ���� ��������� �� ������ � �����.
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

				for (_int i = 0; i < ChainsCount; i++)
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

	// ��������� ������������ �����, �� �������� �� ��� ������� CheckedPos.
	inline bool CheckClosure(const _int StartPos, const _int CheckedPos)
	{
		_int InpChainPoints[4], InpSurPoints[4];

		GameStack<_int, MAX_CHAIN_POINTS> Chains[4];
		_int InsidePoints[4];

		// ���� ������, ����� �������� �����������.
		_int OutPlayer = GetPlayer(StartPos);

		_int InpPointsCount = GetInputPoints(StartPos, OutPlayer | PutBit, InpChainPoints, InpSurPoints);
		if (InpPointsCount > 1)
		{
			_int ChainsCount = BuildChainsFast(StartPos, InpChainPoints, InpSurPoints, InpPointsCount, Chains, InsidePoints);
			for (_int i = 0; i < ChainsCount; i++)
				if (PointInsideRing(CheckedPos, Chains[i]))
				{
					for (_int j = 0; j < Chains[i].Count; j++)
					{
						// ��������� � ������ ��������� ����� �������.
						CurrentBoardChangesNumber++;
						PointsChange.Push(Points[Chains[i].Stack[j]]);
						PointsChange.Push(Chains[i].Stack[j]);
						// �������� ����� �������.
						SetBaseBound(Chains[i].Stack[j]);
					}
					return true;
				}
		}

		return false;
	}

	inline _int GetHash()
	{
		return Hash;
	}

	#if DEBUG
	public:
	// ���������� �����.
	_int DebugPoints[PointsLength22];
	// ������� ����������� ����� � ���������� ��� ����������� ���������.
	void CopyToDebugPoints()
	{
		for (_int i = 0; i < PointsLength22; i++)
			DebugPoints[i] = Points[i];
	}
	// ������� ��������� ����� � ���������� ������.
	bool CompareWithDebugPoints()
	{
		for (_int i = 0; i < PointsLength22; i++)
			if (DebugPoints[i] != Points[i])
				return false;
		return true;
	}

	void Save()
	{
		FILE *f = fopen("debug.txt", "w");
		int x, y;
		fprintf(f, "(SZX[%d] SZY[%d]\n", FieldWidth, FieldHeight);
		for (_int i = 0; i < PointsSeq.Count; i++)
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