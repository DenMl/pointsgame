#pragma once

#include "Config.h"
#include "BasicTypes.h"
#include "BasicConstants.h"
#include "Zobrist.h"
#include "AuxFunc.h"
#include "Player.h"
#include "static_vector.h"
#include <stack>
#include <functional>

using namespace std;
using std::tr1::function;

class Field
{
#pragma region State bits and masks
private:

	// ���, ����������� ����� ������.
	static const value PlayerBit = 0x1;
	// ���, ����������� �� ������� ����� � ����.
	static const value PutBit = 0x2;
	// ���, ����������� �� ������������� �����.
	static const value SurBit = 0x4;
	// ���, ����������� �� ��, ����������� �� ���-������ ����� �� ����.
	static const value BoundBit = 0x8;
	// ���, ����������� �� ������ ����.
	static const value EmptyBaseBit = 0x10;
	// ��� ��� ��������� ������� �����.
	static const value TagBit = 0x20;
	// ���, ������� ���������� ������� ����.
	static const value BadBit = 0x40;

	// ����� ��� ����������� �������.
	static const value EnableMask = BadBit | SurBit | PutBit | PlayerBit;
	static const value BoundMask = BadBit | BoundBit | SurBit | PutBit | PlayerBit;
#pragma endregion

#pragma region Get and set state functions
public:
	// Get state functions.
	// ������� ��������� ���������.

	// �������� �� ���������� ������, ��� ����� ��� ����������.
	inline player GetPlayer(const pos Pos) const { return Points[Pos] & PlayerBit; }
	// ��������� �� ����������, ���������� �� ��� �����.
	inline bool IsPutted(const pos Pos) const { return (Points[Pos] & PutBit) != 0; }
	// �������� �� ����������, �������� �� ����� ���������� ����.
	inline bool IsBaseBound(const pos Pos) const { return (Points[Pos] & BoundBit) != 0; }
	// ��������� �� ����������, ��������� �� ����.
	inline bool IsCaptured(const pos Pos) const { return (Points[Pos] & SurBit) != 0; }
	// ��������� �� ����������, ����� �� ��� � ������ ����.
	inline bool IsInEmptyBase(const pos Pos) const { return (Points[Pos] & EmptyBaseBit) != 0; }
	// ��������� �� ����������, �������� �� ����.
	inline bool IsTagged(const pos Pos) const { return (Points[Pos] & TagBit) != 0; }
	// �������� ������� �� ����������.
	inline value GetEnableCond(const pos Pos) const { return Points[Pos] & EnableMask; }
	// �������� ����������� ���� �� �������.
	inline bool IsEnable(const pos Pos, const value EnableCond) const { return (Points[Pos] & EnableMask) == EnableCond; }
	// �������� ��������� ���� �� �������.
	inline bool IsNotEnable(const pos Pos, const value EnableCond) const { return (Points[Pos] & EnableMask) != EnableCond; }
	// �������� �� ��, ��������� �� ����.
	inline bool IsBound(const pos Pos, const value BoundCond) const { return (Points[Pos] & BoundMask) == BoundCond; }
	// �������� �� ��, �� ��������� �� ����.
	inline bool IsNotBound(const pos Pos, const value BoundCond) const { return (Points[Pos] & BoundMask) != BoundCond; }
	// ������� �� ��, �������� �� ��������� ����� � �����.
	inline bool PuttingAllow(const pos Pos) const { return !(Points[Pos] & (PutBit | SurBit | BadBit)); }

	// Set state functions.
	// ������� ��������� ���������.

	// �������� ���� �� ���������� ��� ���������� �����.
	inline void SetPutted(const pos Pos) { Points[Pos] |= PutBit; }
	// ������ � ���� �� ���������� PutBit.
	inline void ClearPutBit(const pos Pos) { Points[Pos] &= ~PutBit; }
	// �������� ���� �� ���������� ��� ������������� ������.
	inline void SetPlayer(const pos Pos, const player Player) { Points[Pos] = (Points[Pos] & ~PlayerBit) | Player; }
	// �������� ���� �� ���������� ��� ���������� ����� ������.
	inline void SetPlayerPutted(const pos Pos, const player Player) { Points[Pos] |= Player | PutBit; }
	// �������� ����� SurBit (���������).
	inline void Capture(const pos Pos) { Points[Pos] |= SurBit; }
	// ������ ��� SurBit.
	inline void Free(const pos Pos) { Points[Pos] &= ~SurBit; }
	// �������� ����� ��� ���������� ����.
	inline void SetBaseBound(const pos Pos) { Points[Pos] |= BoundBit; }
	// �������� ����� ��� �� ���������� ����.
	inline void ClearBaseBound(const pos Pos) { Points[Pos] &= ~BoundBit; }
	inline void SetInEmptyBase(const pos Pos, const player Player) { Points[Pos] &= ~PlayerBit | EmptyBaseBit | Player; }
	inline void SetEmptyBase(const pos Pos) { Points[Pos] |= EmptyBaseBit; }
	inline void ClearEmptyBase(const pos Pos) { Points[Pos] &= ~EmptyBaseBit; }
	// ���������� ��� TagBit.
	inline void SetTag(const pos Pos) { Points[Pos] |= TagBit; }
	// ������ ��� TagBit.
	inline void ClearTag(const pos Pos) { Points[Pos] &= ~TagBit; }
#pragma endregion

#pragma region MainVariables
public:
	static_vector<board_change, MAX_CHAIN_POINTS> Changes;

	// Main points array (game board).
	// �������� ������ ����� (������� �����).
	value Points[PointsLength22];

	// Real field width.
	// �������������� ������ ����.
	coord Width;
	// Real field height.
	// �������������� ������ ����.
	coord Height;

	// History points sequance.
	// ������������������ ������������ �����.
	static_vector<pos, MAX_CHAIN_POINTS> PointsSeq;

	// ������� ��������� ������ ���.
	// SurStandart = 0 - ���� PlayerRed ������ � ������ ���� � ������ �� �������, �� PlayerBlack ������� ��� ����������.
	// SurAlways = 1 - �������� ����, ���� ���� ��� ��������� ����� ������.
	// SurAlwaysEnemy = 2 - ������� ������ PlayerBlack, ���� PlayerRed �������� ����� � ������ ����.
	sur_cond SurCond;

	// ������� ������ ����.
	coord ExpandWidth, ExpandHeight;
	// �������� �������� ���� (������ => 0; �������� => 1).
	coord ParityWidth, ParityHeight;

	// Current player color.
	// ������� ���� �������.
	player CurPlayer;

	// Capture points count.
	// ���������� ����������� �����.
	score CaptureCount[2];

	score DCaptureCount, DFreedCount;

	size_t Hash;

	public:
	// ����������, ��������������� ������ �������� � ������� ������� ����� �����.
	pos MinPos, MaxPos;
#pragma endregion

private:
	// ���������� ����� ������������ �������� Pos1 � Pos2.
	inline static int Square(const pos Pos1, const pos Pos2);
	//  * . .   x . *   . x x   . . .
	//  . o .   x o .   . o .   . o x
	//  x x .   . . .   . . *   * . x
	//  o - center pos
	//  x - pos
	//  * - result
	inline void GetFirstNextPos(const pos CenterPos, pos &Pos) const;
	//  . . .   * . .   x * .   . x *   . . x   . . .   . . .   . . .
	//  * o .   x o .   . o .   . o .   . o *   . o x   . o .   . o .
	//  x . .   . . .   . . .   . . .   . . .   . . *   . * x   * x .
	//  o - center pos
	//  x - pos
	//  * - result
	inline void GetNextPos(const pos CenterPos, pos &Pos) const;
	// ���������� ���������� ����� ����� ����� � CenterPos.
	// InpChainPoints - ��������� ����� �����, InpSurPoints - ��������� ���������� �����.
	inline short GetInputPoints(const pos CenterPos, const value EnableCond, pos InpChainPoints[], pos InpSurPoints[]) const;
	// ��������� ��������� �����.
	void PlaceBeginPattern(begin_pattern cur_pattern);
	// ��������� ����� �������.
	inline void AddSubCapturedFreed(const player Player, const score Captured, const score Freed);
	// �������� Captured/Free � ����������� �� ����, ��������� ��� �������� �����.
	inline void CheckCapturedAndFreed(const pos Pos, const player Player, score &Captured, score &Freed);
	// �����������/����������� ���������� �����.
	inline void SetCaptureFreeState(const pos Pos, const player Player);
	inline void Wave(pos StartPos, function<bool(pos)> Condition, function<void(pos)> Action);
	// ������� ������� ������ ���� � ���� �����, ������� � ������� StartPos.
	void RemoveEmptyBase(const pos StartPos);
	bool BuildChain(const pos StartPos, const value EnableCond, const pos DirectionPos, static_vector<pos, MAX_CHAIN_POINTS> &Chain);
	void FindSurround(static_vector<pos, MAX_CHAIN_POINTS> &Chain, pos InsidePoint, player Player);
	inline void UpdateHash(player Player, short Surrounded, pos Pos);
	inline intersection_state GetIntersectionState(const pos Pos, const pos NextPos) const;

public:
	// �����������.
	Field(const coord FieldWidth, const coord FieldHeight, const sur_cond SurCond, const begin_pattern BeginPattern);
	// ����������� �����������.
	Field(const Field &Orig);

	inline int GetScore(player Player) const
	{
		return CaptureCount[Player] - CaptureCount[NextPlayer(Player)];
	}

	// ������������ ������� ����������� ���������.
	inline pos ConvertToPos(const coord X, const coord Y) const
	{
		return (Y + ExpandHeight) * FieldWidth2 + X + ExpandWidth;
	}
	// ����������� �� Pos � XY.
	inline void ConvertToXY(const pos Pos, coord &X, coord &Y) const
	{
		X = static_cast<coord>(Pos % FieldWidth2 - ExpandWidth);
		Y = static_cast<coord>(Pos / FieldWidth2 - ExpandHeight);
	}

	// ��������� ����� �� ���� ���������� �� ������� ������.
	inline bool DoStep(const pos Pos)
	{
		if (PuttingAllow(Pos))
		{
			DoUnsafeStep(Pos);
			return true;
		}
		return false;
	}
	// ��������� ����� �� ����.
	inline bool DoStep(const pos Pos, const player Player)
	{
		if (PuttingAllow(Pos))
		{
			DoUnsafeStep(Pos, Player);
			return true;
		}
		return false;
	}
	// ��������� ����� �� ���� ����������� ������ (��� �������������� ��������).
	inline void DoUnsafeStep(const pos Pos)
	{
		Changes.resize(Changes.size() + 1);
		Changes.back().capture_count[0] = CaptureCount[0];
		Changes.back().capture_count[1] = CaptureCount[1];
		Changes.back().player = CurPlayer;

		// ��������� � ��������� ������������ �����.
		Changes.back().changes.push(pair<pos, value>(Pos, Points[Pos]));

		SetPlayer(Pos, CurPlayer);
		SetPutted(Pos);

		PointsSeq.push_back(Pos);

		CheckClosure(Pos, CurPlayer);

		SetNextPlayer();
	}
	// ��������� ����� �� ���� ���������� �� ������� ������ ����������� ������ (��� �������������� ��������).
	inline void DoUnsafeStep(const pos Pos, const player Player)
	{
		Changes.resize(Changes.size() + 1);
		Changes.back().capture_count[0] = CaptureCount[0];
		Changes.back().capture_count[1] = CaptureCount[1];
		Changes.back().player = CurPlayer;

		// ��������� � ��������� ������������ �����.
		Changes.back().changes.push(pair<pos, value>(Pos, Points[Pos]));

		SetPlayer(Pos, Player);
		SetPutted(Pos);

		PointsSeq.push_back(Pos);

		CheckClosure(Pos, Player);
	}

	// ������ ��� � ��������� �� ������������ ������ ����� CheckedPos.
	inline bool DoUnsafeStepAndCheckPoint(const pos Pos, const player Player, const pos CheckedPos)
	{
		Changes.resize(Changes.size() + 1);
		Changes.back().capture_count[0] = CaptureCount[0];
		Changes.back().capture_count[1] = CaptureCount[1];
		Changes.back().player = CurPlayer;

		// ��������� � ��������� ������������ �����.
		Changes.back().changes.push(pair<pos, value>(Pos, Points[Pos]));

		SetPlayer(Pos, Player);
		SetPutted(Pos);

		PointsSeq.push_back(Pos);
		
		return CheckClosure(Pos, CheckedPos);
	}

	// ����� ����.
	inline void UndoStep()
	{
		PointsSeq.pop_back();
		while (!Changes.back().changes.empty())
		{
			Points[Changes.back().changes.top().first] = Changes.back().changes.top().second;
			Changes.back().changes.pop();
		}
		CurPlayer = Changes.back().player;
		CaptureCount[0] = Changes.back().capture_count[0];
		CaptureCount[1] = Changes.back().capture_count[1];
		Changes.pop_back();
	}

	// ���������� ���������� ������ ��� ��������.
	inline void SetNextPlayer()
	{
		CurPlayer = NextPlayer(CurPlayer);
	}

	inline void SetPlayer(const player Player)
	{
		CurPlayer = Player;
	}

	// ���������, ��������� �� ��� ����� �����.
	inline static bool IsNear(const pos Pos1, const pos Pos2)
	{
		pos TempPos = Pos1 - FieldWidth2 - 1;
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
	inline bool IsNearPoints(const pos Pos, const player Player) const
	{
		pos TempPos = Pos - FieldWidth2 - 1;
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
	inline short NumberNearPoints(const pos Pos, const player Player) const
	{
		short Result = 0;
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
	inline short NumberNearGroups(const pos CenterPos, const player Player) const
	{
		short k = 0;

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

	bool PointInsideRing(const pos TestedPos, const static_vector<pos, MAX_CHAIN_POINTS> &Ring) const
	{
		uint Intersections = 0;

		intersection_state State = IS_NONE;

		for (auto i = Ring.begin(); i < Ring.end(); i++)
		{
			switch (GetIntersectionState(TestedPos, *i))
			{
			case (IS_NONE):
				State = IS_NONE;
				break;
			case (IS_UP):
				if (State == IS_DOWN)
					Intersections++;
				State = IS_UP;
				break;
			case (IS_DOWN):
				if (State == IS_UP)
					Intersections++;
				State = IS_DOWN;
				break;
			}
		}
		if (State == IS_UP || State == IS_DOWN)
		{
			auto i = Ring.begin();
			intersection_state TempState = GetIntersectionState(TestedPos, *i);
			while (TempState == State || TempState == IS_TARGET)
			{
				i++;
				TempState = GetIntersectionState(TestedPos, *i);
			}
			if (TempState != IS_NONE)
				Intersections++;
		}

		return Intersections % 2 == 1;
	}

	// ��������� ������������ ����� �� ������� ��������� �� ���������, � ��������, ���� ��� ����.
	void CheckClosure(const pos StartPos, player Player)
	{
		short InpPointsCount;
		pos InpChainPoints[4], InpSurPoints[4];

		static_vector<pos, MAX_CHAIN_POINTS> Chain;

		DCaptureCount = 0;
		DFreedCount = 0;

		if (IsInEmptyBase(StartPos)) // ���� ����� ���������� � ������ ����.
		{
			// ���������, � ��� ������ ���� ���������� �����.
			pos Pos = StartPos - 1;
			while (!IsPutted(Pos))
				Pos--;

			if (GetPlayer(Pos) == GetPlayer(StartPos)) // ���� ��������� � ���� ������ ����.
			{
				ClearEmptyBase(StartPos);
				return;
			}

#if SURROUND_CONDITIONS
			if (SurCond != SC_ALWAYS_ENEMY) // ���� ��������� �� ������ � �����.
#endif
			{
				InpPointsCount = GetInputPoints(StartPos, Player | PutBit, InpChainPoints, InpSurPoints);
				if (InpPointsCount > 1)
				{
					short ChainsCount = 0;
					for (short i = 0; i < InpPointsCount; i++)
						if (BuildChain(StartPos, GetPlayer(StartPos) | PutBit, InpChainPoints[i], Chain))
						{
							FindSurround(Chain, InpSurPoints[i], Player);
							ChainsCount++;
							if (ChainsCount == InpPointsCount - 1)
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
				for (short i = 0; i < InpPointsCount; i++)
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
				short ChainsCount = 0;
				for (short i = 0; i < InpPointsCount; i++)
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

	// ��������� ������������ �����, �� �������� �� ��� ������� CheckedPos.
	inline bool CheckClosure(const pos StartPos, const pos CheckedPos)
	{
		pos InpChainPoints[4], InpSurPoints[4];

		static_vector<pos, MAX_CHAIN_POINTS> Chain;

		// ���� ������, ����� �������� �����������.
		short Player = GetPlayer(StartPos);

		short InpPointsCount = GetInputPoints(StartPos, Player | PutBit, InpChainPoints, InpSurPoints);
		if (InpPointsCount > 1)
		{
			for (ushort i = 0; i < InpPointsCount; i++)
				if (BuildChain(StartPos, GetPlayer(StartPos) | PutBit, InpChainPoints[i], Chain))
					if (PointInsideRing(CheckedPos, Chain))
					{
						for (auto j = Chain.begin(); j < Chain.end(); j++)
						{
							// ��������� � ������ ��������� ����� �������.
							Changes.back().changes.push(pair<pos, value>(*j, Points[*j]));
							// �������� ����� �������.
							SetBaseBound(*j);
						}
						return true;
					}
		}

		return false;
	}

	inline size_t GetHash()
	{
		return Hash;
	}
};


inline int Field::Square(const pos Pos1, const pos Pos2)
{
	return (Pos1 % FieldWidth2) * (Pos2 / FieldWidth2) - (Pos1 / FieldWidth2) * (Pos2 % FieldWidth2);
}
inline void Field::GetFirstNextPos(const pos CenterPos, pos &Pos) const
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
inline void Field::GetNextPos(const pos CenterPos, pos &Pos) const
{
	pos t;
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
inline short Field::GetInputPoints(const pos CenterPos, const value EnableCond, pos InpChainPoints[], pos InpSurPoints[]) const
{
	short k = 0;

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
inline void Field::AddSubCapturedFreed(const player Player, const score Captured, const score Freed)
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
inline void Field::CheckCapturedAndFreed(const pos Pos, const player Player, score &Captured, score &Freed)
{
	if (IsPutted(Pos))
	{
		if (GetPlayer(Pos) != Player)
			Captured++;
		else if (IsCaptured(Pos))
			Freed++;
	}
}
inline void Field::SetCaptureFreeState(const pos Pos, const player Player)
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
inline void Field::Wave(pos StartPos, function<bool(pos)> Condition, function<void(pos)> Action)
{
	// ������� ��� ��������� ��������� (������ � ������).
	static_vector<pos, MAX_CHAIN_POINTS> q;

	q.push_back(StartPos);
	SetTag(StartPos);
	auto it = q.begin();
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
inline void Field::UpdateHash(player Player, short Surrounded, pos Pos)
{
	Hash ^= GetZobristHash(Player, Surrounded, Pos);
}
inline intersection_state Field::GetIntersectionState(const pos Pos, const pos NextPos) const
{
	point a, b;
	ConvertToXY(Pos, a.x, a.y);
	ConvertToXY(NextPos, b.x, b.y);

	if (b.x <= a.x)
		switch (b.y - a.y)
	{
		case (1):
			return IS_UP;
		case (0):
			return IS_TARGET;
		case (-1):
			return IS_DOWN;
		default:
			return IS_NONE;
	}
	else
		return IS_NONE;
}