#pragma once

#include "Config.h"
#include "BasicTypes.h"
#include "BasicConstants.h"
#include "GameStack.h"
#include "Zobrist.h"
#include "AuxFunc.h"
#include "Player.h"
#include <vector>
#include <queue>
#include <stack>

using namespace std;

enum SurroundCondition
{
	Standart,
	Always,
	AlwaysEnemy
};

enum BeginPattern
{
	CleanPattern,
	CrosswisePattern,
	SquarePattern
};

class BoardChange
{
	uint CaptureCount[2];
	short Player;
	stack<Pair<uint, ushort>> Changes;
};

class Field
{
#pragma region State bits and masks
	private:

	// ���, ����������� ����� ������.
	static const ushort PlayerBit = 0x1;
	// ���, ����������� �� ������� ����� � ����.
	static const ushort PutBit = 0x2;
	// ���, ����������� �� ������������� �����.
	static const ushort SurBit = 0x4;
	// ���, ����������� �� ��, ����������� �� ���-������ ����� � ����.
	static const ushort BoundBit = 0x8;
	// ���, ����������� �� ������ ����.
	static const ushort EmptyBaseBit = 0x10;
	// ��� ��� ��������� ������� �����.
	static const ushort TagBit = 0x20;
	// ���, ������� ���������� ������� ����.
	static const ushort BadValue = 0x40;

	// ����� ��� ����������� �������.
	static const ushort EnableMask = SurBit | PutBit | PlayerBit;
	static const ushort BoundMask =  BoundBit | SurBit | PutBit | PlayerBit;
#pragma endregion

#pragma region Get and set state functions
	public:
	// Get state functions.
	// ������� ��������� ���������.

	// �������� �� ���������� ������, ��� ����� ��� ����������.
	inline const short GetPlayer(const uint Pos) { return Points[Pos] & PlayerBit; }
	// ��������� �� ����������, ���������� �� ��� �����.
	inline const bool IsPutted(const uint Pos) { return (Points[Pos] & PutBit) != 0; }
	// �������� �� ����������, �������� �� ����� ���������� ����.
	inline const bool IsBaseBound(const uint Pos) { return (Points[Pos] & BoundBit) != 0; }
	// ��������� �� ����������, ��������� �� ����.
	inline const bool IsCaptured(const uint Pos) { return (Points[Pos] & SurBit) != 0; }
	// ��������� �� ����������, ����� �� ��� � ������ ����.
	inline const bool IsInEmptyBase(const uint Pos) { return (Points[Pos] & EmptyBaseBit) != 0; }
	// ��������� �� ����������, �������� �� ����.
	inline const bool IsTagged(const uint Pos) { return (Points[Pos] & TagBit) != 0; }
	// �������� ������� �� ����������.
	inline const ushort GetEnableCond(const uint Pos) { return Points[Pos] & EnableMask; }
	// �������� ����������� ���� �� �������.
	inline const bool IsEnable(const uint Pos, const ushort EnableCond) { return (Points[Pos] & EnableMask) == EnableCond; }
	// �������� ��������� ���� �� �������.
	inline const bool IsNotEnable(const uint Pos, const ushort EnableCond) { return (Points[Pos] & EnableMask) != EnableCond; }
	// �������� �� ��, ��������� �� ����.
	inline const bool IsBound(const uint Pos, const ushort BoundCond) { return (Points[Pos] & BoundMask) == BoundCond; }
	// �������� �� ��, �� ��������� �� ����.
	inline const bool IsNotBound(const uint Pos, const ushort BoundCond) { return (Points[Pos] & BoundMask) != BoundCond; }
	// ������� �� ��, �������� �� ��������� ����� � �����.
	inline const bool PuttingAllow(const uint Pos) { return !(Points[Pos] & (PutBit | SurBit | BadValue)); }

	// Set state functions.
	// ������� ��������� ���������.

	// �������� ���� �� ���������� ��� ���������� �����.
	inline void SetPutted(const uint Pos) { Points[Pos] |= PutBit; }
	// ������ � ���� �� ���������� PutBit.
	inline void ClearPutBit(const uint Pos) { Points[Pos] &= ~PutBit; }
	// �������� ���� �� ���������� ��� ������������� ������.
	inline void SetPlayer(const uint Pos, const short Player) { Points[Pos] = (Points[Pos] & ~PlayerBit) | Player; }
	// �������� ���� �� ���������� ��� ���������� ����� ������.
	inline void SetPlayerPutted(const uint Pos, const short Player) { Points[Pos] |= Player | PutBit; }
	// �������� ����� SurBit (���������).
	inline void Capture(const uint Pos) { Points[Pos] |= SurBit; }
	// ������ ��� SurBit.
	inline void Free(const uint Pos) { Points[Pos] &= ~SurBit; }
	// �������� ����� ��� ���������� ����.
	inline void SetBaseBound(const uint Pos) { Points[Pos] |= BoundBit; }
	// �������� ����� ��� �� ���������� ����.
	inline void ClearBaseBound(const uint Pos) { Points[Pos] &= ~BoundBit; }
	inline void SetInEmptyBase(const uint Pos, const short Player) { Points[Pos] &= ~PlayerBit | EmptyBaseBit | Player; }
	inline void SetEmptyBase(const uint Pos) { Points[Pos] |= EmptyBaseBit; }
	inline void ClearEmptyBase(const uint Pos) { Points[Pos] &= ~EmptyBaseBit; }
	// ���������� ��� TagBit.
	inline void SetTag(const uint Pos) { Points[Pos] |= TagBit; }
	// ������ ��� TagBit.
	inline void ClearTag(const uint Pos) { Points[Pos] &= ~TagBit; }
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
	GameStack<ulong, MAX_CHAIN_POINTS * MAX_CHAIN_POINTS> PointsChange;

	// ���������� ��������� ����� �� ������� ���.
	uint CurrentBoardChangesNumber;

	// Main points array (game board).
	// �������� ������ ����� (������� �����).
	ushort Points[PointsLength22];

	// Real field width.
	// �������������� ������ ����.
	ushort FieldWidth;
	// Real field height.
	// �������������� ������ ����.
	ushort FieldHeight;

	// History points sequance.
	// ������������������ ������������ �����.
	vector<uint> *PointsSeq;

	// ������� ��������� ������ ���.
	// SurStandart = 0 - ���� PlayerRed ������ � ������ ���� � ������ �� �������, �� PlayerBlack ������� ��� ����������.
	// SurAlways = 1 - �������� ����, ���� ���� ��� ��������� ����� ������.
	// SurAlwaysEnemy = 2 - ������� ������ PlayerBlack, ���� PlayerRed �������� ����� � ������ ����.
	SurroundCondition SurCond;

	// ������������ ������ � ������ ����.
	// Crosswise = 0 - Crosswise.
	// Clean = 1 - Clean.
	// Square = 2 - Square.
	BeginPattern Pattern;

	// ������� ������ ����.
	ushort ExpandWidth, ExpandHeight;
	// �������� �������� ���� (������ => 0; �������� => 1).
	ushort ParityWidth, ParityHeight;

	// Current player color.
	// ������� ���� �������.
	short CurPlayer, EnemyPlayer;

	// Capture points count.
	// ���������� ����������� �����.
	uint CaptureCount[2];

	int DCaptureCount, DFreedCount;

	ulong Hash;

	public:
	// ����������, ��������������� ������ �������� � ������� ������� ����� �����.
	uint MinPos, MaxPos;
#pragma endregion

	private:
	// ���������� ����� ������������ �������� Pos1 � Pos2.
	inline static int Square(const uint Pos1, const uint Pos2)
	{
		return (Pos1 % FieldWidth2) * (Pos2 / FieldWidth2) - (Pos1 / FieldWidth2) * (Pos2 % FieldWidth2);
	}

	//  * . .   x . *   . x x   . . .
	//  . o .   x o .   . o .   . o x
	//  x x .   . . .   . . *   * . x
	//  o - center pos
	//  x - pos
	//  * - result
	inline const void GetFirstNextPos(const uint CenterPos, uint &Pos)
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
	inline const void GetNextPos(const uint CenterPos, uint &Pos)
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

	// ���������� ���������� ����� ����� ����� � CenterPos.
	// InpChainPoints - ��������� ����� �����, InpSurPoints - ��������� ���������� �����.
	inline const ushort GetInputPoints(const uint CenterPos, const ushort EnableCond, uint *InpChainPoints, uint *InpSurPoints)
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

	// ��������� ��������� �����.
	inline void PlaceBeginPattern()
	{
		Point point;
		switch (Pattern)
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
	inline void AddSubCapturedFreed(const short Player, const int Captured, const int Freed)
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

	// �������� Captured/Free � ����������� �� ����, ��������� ��� �������� �����.
	inline void CheckCapturedAndFreed(const uint Pos, const short Player, int &Captured, int &Freed)
	{
		if (IsPutted(Pos))
		{
			if (GetPlayer(Pos) != Player)
				Captured++;
			else if (IsCaptured(Pos))
				Freed++;
		}
	}

	// �����������/����������� ���������� �����.
	inline void SetCaptureFreeState(const uint Pos, const short Player)
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
	inline void CapturedAndFreedCount(const uint StartPos, const short Player, int &Captured, int &Freed, GameStack<uint, MAX_SUR_POINTS> &SurPoints)
	{
		GameStack<uint, MAX_CHAIN_POINTS> TempStack;

		SurPoints.Clear();

		Captured = 0;
		Freed = 0;
		ushort BoundCond = Player | PutBit | BoundBit;
		TempStack.Clear();
		TempStack.Push(StartPos);
		SetTag(StartPos);

		while (TempStack.Count != 0)
		{
			uint Pos = TempStack.Pop();
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
	inline void RemoveEmptyBase(const uint StartPos)
	{
		GameStack<uint, MAX_CHAIN_POINTS> TempStack;

		uint Pos = StartPos;
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

	bool BuildChain(const uint StartPos, const uint InpChainPoint, GameStack<uint, MAX_CHAIN_POINTS> &Chain)
	{
		short Player = GetPlayer(StartPos);

		uint Pos;

		Chain.Clear();
		Chain.Push(StartPos);
		Pos = InpChainPoint;
		uint CenterPos = StartPos;
		// ������� ����.
		int TempSquare = Square(CenterPos, Pos);
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

		for (uint i = 0; i < Chain.Count; i++)
			ClearTag(Chain.Stack[i]);

		return (TempSquare < 0 && Chain.Count > 2);
	}

	inline ushort BuildChains(const uint StartPos, const uint InpChainPoints[], const uint InpSurPoints[], const ushort InpPointsCount, GameStack<uint, MAX_CHAIN_POINTS> Chains[], uint InsidePoints[])
	{
		ushort Count = 0;
		for (ushort i = 0; i < InpPointsCount; i++)
			if (BuildChain(StartPos, InpChainPoints[i], Chains[Count]))
			{
				InsidePoints[Count] = InpSurPoints[i];
				Count++;
			}

			return Count;
	}

	inline ushort BuildChainsFast(const uint StartPos, const uint InpChainPoints[], const uint InpSurPoints[], const ushort InpPointsCount, GameStack<uint, MAX_CHAIN_POINTS> Chains[], uint InsidePoints[])
	{
		ushort Count = 0;
		for (ushort i = 0; i < InpPointsCount; i++)
			if (BuildChain(StartPos, InpChainPoints[i], Chains[Count]))
			{
				InsidePoints[Count] = InpSurPoints[i];
				Count++;

				if (Count == InpPointsCount - 1)
					break;
			}

			return Count;
	}

	void FindSurround(GameStack<uint, MAX_CHAIN_POINTS> &Chain, uint InsidePoint, short Player)
	{
		// ���������� ����������� �����.
		int CurCaptureCount = 0;
		// ���������� ����������� ������ �����.
		int CurFreedCount = 0;

		GameStack<uint, MAX_SUR_POINTS> SurPoints;

		// �������� ����� �������.
		for (uint i = 0; i < Chain.Count; i++)
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
			for (uint i = 0; i < Chain.Count; i++)
			{
				ClearTag(Chain.Stack[i]);
				// ��������� � ������ ��������� ����� �������.
				CurrentBoardChangesNumber++;
				PointsChange.Push(Points[Chain.Stack[i]]);
				PointsChange.Push(Chain.Stack[i]);
				// �������� ����� �������.
				SetBaseBound(Chain.Stack[i]);
			}

			for (uint j = 0; j < SurPoints.Count; j++)
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
			for (uint i = 0; i < Chain.Count; i++)
				ClearTag(Chain.Stack[i]);

			for (uint i = 0; i < SurPoints.Count; i++)
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

	inline void FindSurrounds(GameStack<uint, MAX_CHAIN_POINTS> Chains[], uint InsidePoints[], ushort ChainsCount, short Player)
	{
		for (ushort i = 0; i < ChainsCount; i++)
			FindSurround(Chains[i], InsidePoints[i], Player);
	}

	inline void UpdateHash(short Player, short Surrounded, uint Pos)
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
	const IntersectionState GetIntersectionState(const uint Pos, const uint NextPos)
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
	inline Field(const ushort FieldWidth, const ushort FieldHeight, const SurroundCondition SurCond, const BeginPattern BeginPattern)
	{
		// ������� ������ ��������� �����.
		PointsChange.Clear();
		// ������� ���������� ������� ���������.
		CurrentBoardChangesNumber = 0;

		ExpandWidth = (FieldWidth2 - FieldWidth) / 2;
		ParityWidth = (FieldWidth2 - FieldWidth) % 2;
		ExpandHeight = (FieldHeight2 - FieldHeight) / 2;
		ParityHeight = (FieldHeight2 - FieldHeight) % 2;

		// ������� ����� ����� �������� ������ BadValue.
		for (uint i = 0; i < ExpandHeight * FieldWidth2; i++)
			Points[i] = BadValue;

		uint Pos = (ExpandHeight - 1) * FieldWidth2 + ExpandWidth + FieldWidth;
		for (ushort i = 0; i < FieldHeight; i++)
		{
			// ������� ����� ����� �������� ������ BadValue.
			for (ushort j = 0; j < ExpandWidth * 2 + ParityWidth; j++)
				Points[Pos++] = BadValue;
			// ������� ����������� ����� �����.
			for (ushort j = 0; j < FieldWidth; j++)
				Points[Pos++] = 0;
		}
		// ������ ����� ����� (� ������ ������ ������� �����) �������� ������ BadValue.
		for (Pos; Pos < PointsLength22; Pos++)
			Points[Pos] = BadValue;

		this->FieldWidth = FieldWidth;
		this->FieldHeight = FieldHeight;
		PointsSeq = new vector<uint>();
#if SURROUND_CONDITIONS
		this->SurCond = SurCond;
#endif
		this->Pattern = BeginPattern;
		CurPlayer = PlayerRed;
		EnemyPlayer = PlayerBlack;
		CaptureCount[0] = 0;
		CaptureCount[1] = 0;

		Hash = 0;

		MinPos = UnsafeConvertToPos(0, 0);
		MaxPos = UnsafeConvertToPos(FieldWidth - 1, FieldHeight - 1);

		PlaceBeginPattern();
	}

	// ����������� �����������.
	inline Field(const Field &Orig)
	{
		PointsChange.Copy(Orig.PointsChange);
		CurrentBoardChangesNumber = Orig.CurrentBoardChangesNumber;

		ExpandWidth = Orig.ExpandWidth;
		ParityWidth = Orig.ParityWidth;
		ExpandHeight = Orig.ExpandHeight;
		ParityHeight = Orig.ParityHeight;

		for (uint i = 0; i < PointsLength22; i++)
			Points[i] = Orig.Points[i];

		FieldWidth = Orig.FieldWidth;
		FieldHeight = Orig.FieldHeight;
		PointsSeq = new vector<uint>(*Orig.PointsSeq);
#if SURROUND_CONDITIONS
		SurCond = Orig.SurCond;
#endif
		Pattern = Orig.Pattern;
		CurPlayer = Orig.CurPlayer;
		EnemyPlayer = Orig.EnemyPlayer;
		CaptureCount[0] = Orig.CaptureCount[0];
		CaptureCount[1] = Orig.CaptureCount[1];

		Hash = Orig.Hash;

		MinPos = Orig.MinPos;
		MaxPos = Orig.MaxPos;
	}

	~Field()
	{
		delete PointsSeq;
	}

	inline const int GetScore(short Player)
	{
		return CaptureCount[Player] - CaptureCount[NextPlayer(Player)];
	}

	// ������� ����������� ���������.
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
	// ������������ ������� ����������� ���������.
	inline const uint UnsafeConvertToPos(const ushort X, const ushort Y)
	{
		return (Y + ExpandHeight) * FieldWidth2 + X + ExpandWidth;
	}
	inline const uint UnsafeConvertToPos(const Point point)
	{
		return UnsafeConvertToPos(point.X, point.Y);
	}
	// ����������� �� Pos � XY.
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

	// ��������� ����� �� ���� ���������� �� ������� ������.
	inline bool DoStep(const uint Pos)
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
	inline bool DoStep(const uint Pos, const short Player)
	{
		if (PuttingAllow(Pos))
		{
			DoUnsafeStep(Pos, Player);
			return true;
		}
		return false;
	}
	inline bool DoStep(const Point point, const short Player)
	{
		return DoStep(ConvertToPos(point), Player);
	}
	// ��������� ����� �� ���� ����������� ������ (��� �������������� ��������).
	inline void DoUnsafeStep(const uint Pos)
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

		PointsSeq->push_back(Pos);

		SetNextPlayer();

		CheckClosure(Pos);

		PointsChange.Push(CurrentBoardChangesNumber);
	}
	inline void DoUnsafeStep(const Point point)
	{
		DoStep(UnsafeConvertToPos(point));
	}
	// ��������� ����� �� ���� ���������� �� ������� ������ ����������� ������ (��� �������������� ��������).
	inline void DoUnsafeStep(const uint Pos, const short Player)
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

		PointsSeq->push_back(Pos);

		CheckClosure(Pos);

		PointsChange.Push(CurrentBoardChangesNumber);
	}
	inline void DoUnsafeStep(const Point point, const short Player)
	{
		DoStep(UnsafeConvertToPos(point), Player);
	}

	// ������ ��� � ��������� �� ������������ ������ ����� CheckedPos.
	inline bool DoUnsafeStepAndCheckPoint(const uint Pos, const short Player, const uint CheckedPos)
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

		PointsSeq->push_back(Pos);
		
		bool result = CheckClosure(Pos, CheckedPos);

		PointsChange.Push(CurrentBoardChangesNumber);

		return result;
	}

	// ����� ����.
	inline void UndoStep()
	{
		PointsSeq->pop_back();
		uint ChangeNumber = PointsChange.Pop();
		for (uint i = 0; i < ChangeNumber; i++)
		{
			uint temp = PointsChange.Pop();
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

	inline void SetCurrentPlayer(const short Player)
	{
		CurPlayer = Player;
		EnemyPlayer = NextPlayer(Player);
	}

	// ���������, ��������� �� ��� ����� �����.
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

	// ���������, ���� �� ����� � Pos ����� ����� Player.
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

	// ���������� ���������� ����� ����� � Pos ����� Player.
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

	// ���������� ���������� ����� ����� ����� � CenterPos.
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

	const bool PointInsideRing(const uint TestedPos, const GameStack<uint, MAX_CHAIN_POINTS> &Ring)
	{
		uint Intersections = 0;

		IntersectionState State = ISNone;

		for (uint i = 0; i < Ring.Count; i++)
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
			uint i = 0;
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
	inline void CheckClosure(const uint StartPos)
	{
		ushort InpPointsCount;
		uint InpChainPoints[4], InpSurPoints[4];

		GameStack<uint, MAX_CHAIN_POINTS> Chains[4];
		uint InsidePoints[4];
		ushort ChainsCount;

		DCaptureCount = 0;
		DFreedCount = 0;

		// ���� ������, ����� �������� �����������.
		short OutPlayer = GetPlayer(StartPos);

		if (IsInEmptyBase(StartPos)) // ���� ����� ���������� � ������ ����.
		{
			// ���������, � ��� ������ ���� ���������� �����.
			uint Pos = StartPos - 1;
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

				for (ushort i = 0; i < ChainsCount; i++)
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
	inline bool CheckClosure(const uint StartPos, const uint CheckedPos)
	{
		uint InpChainPoints[4], InpSurPoints[4];

		GameStack<uint, MAX_CHAIN_POINTS> Chains[4];
		uint InsidePoints[4];

		// ���� ������, ����� �������� �����������.
		short OutPlayer = GetPlayer(StartPos);

		ushort InpPointsCount = GetInputPoints(StartPos, OutPlayer | PutBit, InpChainPoints, InpSurPoints);
		if (InpPointsCount > 1)
		{
			ushort ChainsCount = BuildChainsFast(StartPos, InpChainPoints, InpSurPoints, InpPointsCount, Chains, InsidePoints);
			for (ushort i = 0; i < ChainsCount; i++)
				if (PointInsideRing(CheckedPos, Chains[i]))
				{
					for (uint j = 0; j < Chains[i].Count; j++)
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

	inline ulong GetHash()
	{
		return Hash;
	}
};