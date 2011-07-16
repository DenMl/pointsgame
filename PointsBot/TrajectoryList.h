#pragma once

#include "BasicTypes.h"
#include "BasicConstants.h"
#include "Zobrist.h"
#include "Field.h"
#include "GameStack.h"
#include <assert.h>
#include "Config.h"

const _int MAX_TRAJECTORY_LENGTH = 8;
const _int MAX_TRAJECTORIES_COUNT = 256;

struct Trajectory
{
	_int Points[MAX_TRAJECTORY_LENGTH];
	_int Count;

	_int Hash;

	bool Excluded;

	inline void Clear()
	{
		Count = 0;
		Hash = 0;
		Excluded = false;
	}

	// ��������� ����� � ����������.
	inline void Push(_int Pos)
	{
		Points[Count] = Pos;
		Count++;
		Hash ^= GetZobristHash(Pos);
	}
	// ��������� ����� � ����������, �� �������� ���.
	inline void FastPush(_int Pos)
	{
		Points[Count] = Pos;
		Count++;
	}

	inline void Copy(const Trajectory &Orig)
	{
		for (_int i = 0; i < Orig.Count; i++)
			Points[i] = Orig.Points[i];
		Count = Orig.Count;
		Hash = Orig.Hash;
		Excluded = Orig.Excluded;
	}
	inline void Copy(const Trajectory &Orig, const _int Pos)
	{
		Count = 0;
		for (_int i = 0; i < Orig.Count; i++)
			if (Orig.Points[i] != Pos)
				FastPush(Orig.Points[i]);
		Hash = Orig.Hash ^ GetZobristHash(Pos);
		Excluded = Orig.Excluded;
	}

	inline const _int Find(const _int Pos)
	{
		for (_int i = 0; i < Count; i++)
			if (Points[i] == Pos)
				return i;
		return -1;
	}
};

class TrajectoryList
{
private:
	_int CurrentDepth;

public:
	Trajectory Trajectories[MAX_TRAJECTORIES_COUNT];
	_int Count;

private:
	inline void AddNewTrajectory(Field &CurrentField, const _int Points[], _int Length, _int Player)
	{
		_int TempHash = 0;

		// ������������� ��������.
		// ������ ����� ���������� ������ �������� ���-���� � ����� ����� ���� �� 2 ������ �����.
		// ���� ��� - �� ��������� ��� ����������.
		for (_int i = 0; i < Length; i++)
			if (!CurrentField.IsBaseBound(Points[i]) || (CurrentField.NumberNearGroups(Points[i], Player) < 2))
				return;

		// ����������� ��� ���������� � ���������� � ��� ������������� ��� ���������� ��������.
		for (_int i = 0; i < Length; i++)
			TempHash ^= GetZobristHash(Points[i]);
		for (_int i = 0; i < Count; i++)
			if (TempHash == Trajectories[i].Hash)
				return; // � ������ �������� ��������. ������� ���� �� ������� ������ ��������.

		Trajectories[Count].Excluded = false;
		// ���������� ��� ����� ����������.
		Trajectories[Count].Hash = TempHash;

		for (_int i = 0; i < Length; i++)
		{
			// ��������� ����� � PointsSeq ����������.
			Trajectories[Count].Points[i] = Points[i];
		}
		// ���������� ���������� ����� � ����������.
		Trajectories[Count].Count = Length;
		// ����������� �� 1 ���������� ����������.
		Count++;
	}
	inline void AddNewTrajectory(const Trajectory &CurTrajectory)
	{
		Trajectories[Count].Copy(CurTrajectory);
		Count++;
	}
	// ��������� ����������, ���������� �� CurTrajectory ����������� �� ��� ����� Pos.
	// ����� ������������ ������������ ������� ����������.
	inline void AddNewTrajectory(const Trajectory &CurTrajectory, const _int Pos)
	{
		if (CurTrajectory.Count == 1)
			return;

		Trajectories[Count].Copy(CurTrajectory, Pos);
		Count++;
	}

	// ���������, �� ��� �� ����� ���������� ����� ������� ���, �����, ��������, ����� Pos.
	inline static bool IsTrajectoryValid(Field &CurField, const Trajectory &CurTrajectory, _int Pos)
	{
		for (_int i = 0; i < CurTrajectory.Count; i++)
			if (CurTrajectory.Points[i] != Pos && !CurField.PuttingAllow(CurTrajectory.Points[i]))
				return false;
		return true;
	}
	inline static bool IsTrajectoryValid(Field &CurField, const Trajectory &CurTrajectory)
	{
		for (_int i = 0; i < CurTrajectory.Count; i++)
			if (!CurField.PuttingAllow(CurTrajectory.Points[i]))
				return false;
		return true;
	}

	// ����������� ������� ���������� ����������.
	void BuildTrajectoriesRecursive(Field &CurField, _int Depth, _int Player)
	{
		for (_int Pos = CurField.MinPos; Pos <= CurField.MaxPos; Pos++)
		{
			if (CurField.PuttingAllow(Pos) && CurField.IsNearPoints(Pos, Player))
			{
				if (CurField.IsInEmptyBase(Pos)) // ���� ��������� � ������ ���� (���� ��� ���), �� ������ ������� ���������� ��� �����.
				{
					CurField.DoUnsafeStep(Pos, Player);
					if (CurField.DCaptureCount > 0)
						AddNewTrajectory(CurField, &CurField.PointsSeq.Stack[CurField.PointsSeq.Count - (CurrentDepth - Depth)], CurrentDepth - Depth, Player);
					CurField.UndoStep();
				}
				else
				{
					CurField.DoUnsafeStep(Pos, Player);

#if SURROUND_CONDITIONS
					if (CurField.IsBaseBound(Pos) && CurField.DCaptureCount == 0)
					{
						CurField.UndoStep();
						continue;
					}
#endif

					if (CurField.DCaptureCount > 0)
						AddNewTrajectory(CurField, &CurField.PointsSeq.Stack[CurField.PointsSeq.Count - (CurrentDepth - Depth)], CurrentDepth - Depth, Player);
					else if (Depth > 0)
						BuildTrajectoriesRecursive(CurField, Depth - 1, Player);

					CurField.UndoStep();
				}
			}
		}
	}
	void BuildCurrentTrajectoriesRecursive(Field &CurField, _int Depth, _int Player, _int CheckedPos)
	{
		for (_int Pos = CurField.MinPos; Pos <= CurField.MaxPos; Pos++)
		{
			if (CurField.PuttingAllow(Pos) && CurField.IsNearPoints(Pos, Player))
			{
				if (CurField.DoUnsafeStepAndCheckPoint(Pos, Player, CheckedPos))
					AddNewTrajectory(CurField, &CurField.PointsSeq.Stack[CurField.PointsSeq.Count - (CurrentDepth - Depth)], CurrentDepth - Depth, Player);
				else if (!CurField.IsInEmptyBase(Pos) && Depth > 0)
					BuildCurrentTrajectoriesRecursive(CurField, Depth - 1, Player, CheckedPos);

				CurField.UndoStep();
			}
		}
	}

public:
	// �����������.
	inline TrajectoryList()
	{
		Clear();
	}
	// ����������� �����������.
	inline TrajectoryList(const TrajectoryList &Orig)
	{
		Count = Orig.Count;
		for (_int i = 0; i < Count; i++)
			Trajectories[i].Copy(Orig.Trajectories[i]);
	}

	// ������� ������ ����������.
	inline void Clear()
	{
		Count = 0;
	}

	// ��������� ���������� �� ���� CurrentField ������ Depth �� ������ Player.
	inline void BuildTrajectories(Field &CurField, _int Depth, _int Player)
	{
		CurrentDepth = Depth;
		BuildTrajectoriesRecursive(CurField, Depth - 1, Player);
	}
	// ��������� ���������� �� ���� CurrentField �� ������ ��� ������������ ���������� LastTrajectories.
	inline void BuildEnemyTrajectories(Field &CurField, TrajectoryList &LastTrajectories, _int Pos, _int Depth)
	{
		for (_int i = 0; i < LastTrajectories.Count; i++)
			if ((LastTrajectories.Trajectories[i].Count <= Depth || (LastTrajectories.Trajectories[i].Count == Depth + 1 && LastTrajectories.Trajectories[i].Find(Pos) != -1)) && IsTrajectoryValid(CurField, LastTrajectories.Trajectories[i], Pos))
				AddNewTrajectory(LastTrajectories.Trajectories[i], Pos);
	}

	inline void BuildCurrentTrajectories(Field &CurField, TrajectoryList &LastTrajectories, _int Pos, _int Depth, _int Player)
	{
		for (_int i = 0; i < LastTrajectories.Count; i++)
			if (IsTrajectoryValid(CurField, LastTrajectories.Trajectories[i]))
				AddNewTrajectory(LastTrajectories.Trajectories[i]);

		CurrentDepth = Depth;
		BuildCurrentTrajectoriesRecursive(CurField, Depth - 1, Player, Pos);
	}

	inline void GetPoints(GameStack<_int, MAX_CHAIN_POINTS> &Points)
	{
		for (_int i = 0; i < Count; i++)
			if (!Trajectories[i].Excluded)
				for (_int j = 0; j < Trajectories[i].Count; j++)
					if (Points.Find(Trajectories[i].Points[j]) == -1)
						Points.Push(Trajectories[i].Points[j]);
	}

	// ���������� ���������� �� ����� TrajectoriesBoard (��� ������ ����� Pos ��������� ���������� �������������� TrajectoriesBoard[Pos]).
	// ��� ����������� � ������ ���������� ������� �� ����������� ���������� �� ������������� (���� Excluded).
	inline void Project(_int TrajectoriesBoard[])
	{
		for (_int i = 0; i < Count; i++)
			if (!Trajectories[i].Excluded)
				for (_int j = 0; j < Trajectories[i].Count; j++)
					TrajectoriesBoard[Trajectories[i].Points[j]]++;
	}
	// ������� �������� ���������� � ����� TrajectoriesBoard.
	inline void UnProject(_int TrajectoriesBoard[])
	{
		for (_int i = 0; i < Count; i++)
			if (!Trajectories[i].Excluded)
				for (_int j = 0; j < Trajectories[i].Count; j++)
					TrajectoriesBoard[Trajectories[i].Points[j]]--;
	}

	bool ExcludeUnnecessaryTrajectories(_int TrajectoriesBoard[])
	{
		_int c;
		bool NeedExclude = false;

		for (_int i = 0; i < Count; i++)
		{
			if (Trajectories[i].Excluded)
				continue;
			// ������� � c ���������� �����, �������� ������ � ��� ����������.
			c = 0;
			for (_int j = 0; j < Trajectories[i].Count; j++)
				if (TrajectoriesBoard[Trajectories[i].Points[j]] == 1)
					c++;
			// ���� �����, �������� ������ � ��� ����������, > 1, �� ��������� ��� ����������.
			if (c > 1)
			{
				NeedExclude = true;
				Trajectories[i].Excluded = true;
				for (_int j = 0; j < Trajectories[i].Count; j++)
					TrajectoriesBoard[Trajectories[i].Points[j]]--;
			}
		}

		return NeedExclude;
	}
	inline void IncludeAllTrajectories()
	{
		for (_int i = 0; i < Count; i++)
			Trajectories[i].Excluded = false;
	}

	// ���������� ��� �������� ����������� ���� ����������.
	inline static _int GetIntersectHash(Trajectory &T1, Trajectory &T2)
	{
		_int TempHash = T1.Hash;
		for (_int i = 0; i < T2.Count; i++)
			if (T1.Find(T2.Points[i]) == -1)
				TempHash ^= GetZobristHash(T2.Points[i]);
		return TempHash;
	}

	// ��������� ��������� ����������.
	inline void ExcludeCompositeTrajectories()
	{
		for (_int k = 0; k < Count; k++)
			for (_int i = 0; i < Count - 1; i++)
				if (Trajectories[k].Count > Trajectories[i].Count)
					for (_int j = i + 1; j < Count; j++)
						if (Trajectories[k].Count > Trajectories[j].Count && Trajectories[k].Hash == GetIntersectHash(Trajectories[i], Trajectories[j]))
							Trajectories[k].Excluded = true;
	}
};