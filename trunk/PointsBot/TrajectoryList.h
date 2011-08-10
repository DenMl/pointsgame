#pragma once

#include "BasicTypes.h"
#include "BasicConstants.h"
#include "Zobrist.h"
#include "Field.h"
#include "Config.h"
#include <vector>
#include <algorithm>

const uint MAX_TRAJECTORY_LENGTH = 8;
const uint MAX_TRAJECTORIES_COUNT = 256;

struct Trajectory
{
	uint Points[MAX_TRAJECTORY_LENGTH];
	uint Count;

	ulong Hash;

	bool Excluded;

	inline void Clear()
	{
		Count = 0;
		Hash = 0;
		Excluded = false;
	}

	// Добавляет точку в траекторию.
	inline void Push(uint Pos)
	{
		Points[Count] = Pos;
		Count++;
		Hash ^= GetZobristHash(Pos);
	}
	// Добавляет точку в траекторию, не обновляя хеш.
	inline void FastPush(uint Pos)
	{
		Points[Count] = Pos;
		Count++;
	}

	inline void Copy(const Trajectory &Orig)
	{
		for (uint i = 0; i < Orig.Count; i++)
			Points[i] = Orig.Points[i];
		Count = Orig.Count;
		Hash = Orig.Hash;
		Excluded = Orig.Excluded;
	}
	inline void Copy(const Trajectory &Orig, const uint Pos)
	{
		Count = 0;
		for (uint i = 0; i < Orig.Count; i++)
			if (Orig.Points[i] != Pos)
				FastPush(Orig.Points[i]);
		Hash = Orig.Hash ^ GetZobristHash(Pos);
		Excluded = Orig.Excluded;
	}

	inline const uint Find(const uint Pos)
	{
		for (uint i = 0; i < Count; i++)
			if (Points[i] == Pos)
				return i;
		return -1;
	}
};

class TrajectoryList
{
private:
	uint CurrentDepth;

public:
	Trajectory Trajectories[MAX_TRAJECTORIES_COUNT];
	uint Count;

private:
	inline void AddNewTrajectory(Field &CurrentField, vector<uint>::iterator begin, vector<uint>::iterator end, short Player)
	{
		ulong TempHash = 0;

		// Эвристические проверки.
		// Каждая точка траектории должна окружать что-либо и иметь рядом хотя бы 2 группы точек.
		// Если нет - не добавляем эту траекторию.
		for (vector<uint>::iterator i = begin; i < end; i++)
			if (!CurrentField.IsBaseBound(*i) || (CurrentField.NumberNearGroups(*i, Player) < 2))
				return;

		// Высчитываем хеш траектории и сравниваем с уже существующими для исключения повторов.
		for (vector<uint>::iterator i = begin; i < end; i++)
			TempHash ^= GetZobristHash(*i);
		for (uint i = 0; i < Count; i++)
			if (TempHash == Trajectories[i].Hash)
				return; // В теории возможны коллизии. Неплохо было бы сделать точную проверку.

		Trajectories[Count].Excluded = false;
		// Запоминаем хеш новой траектории.
		Trajectories[Count].Hash = TempHash;

		Trajectories[Count].Clear();
		for (vector<uint>::iterator i = begin; i < end; i++)
		{
			// Добавляем точку в PointsSeq траектории.
			Trajectories[Count].Push(*i);
		}
		// Увеличиваем на 1 количество траекторий.
		Count++;
	}
	inline void AddNewTrajectory(const Trajectory &CurTrajectory)
	{
		Trajectories[Count].Copy(CurTrajectory);
		Count++;
	}
	// Добавляет траекторию, полученную из CurTrajectory исключением из нее точки Pos.
	// Также контролирует недобавление нулевых траекторий.
	inline void AddNewTrajectory(const Trajectory &CurTrajectory, const uint Pos)
	{
		if (CurTrajectory.Count == 1)
			return;

		Trajectories[Count].Copy(CurTrajectory, Pos);
		Count++;
	}

	// Проверяет, во все ли точки траектории можно сделать ход, кроме, возможно, точки Pos.
	inline static bool IsTrajectoryValid(Field &CurField, const Trajectory &CurTrajectory, uint Pos)
	{
		for (uint i = 0; i < CurTrajectory.Count; i++)
			if (CurTrajectory.Points[i] != Pos && !CurField.PuttingAllow(CurTrajectory.Points[i]))
				return false;
		return true;
	}
	inline static bool IsTrajectoryValid(Field &CurField, const Trajectory &CurTrajectory)
	{
		for (uint i = 0; i < CurTrajectory.Count; i++)
			if (!CurField.PuttingAllow(CurTrajectory.Points[i]))
				return false;
		return true;
	}

	// Рекурсивная функция построения траекторий.
	void BuildTrajectoriesRecursive(Field &CurField, uint Depth, short Player)
	{
		for (uint Pos = CurField.MinPos; Pos <= CurField.MaxPos; Pos++)
		{
			if (CurField.PuttingAllow(Pos) && CurField.IsNearPoints(Pos, Player))
			{
				if (CurField.IsInEmptyBase(Pos)) // Если поставили в пустую базу (свою или нет), то дальше строить траекторию нет нужды.
				{
					CurField.DoUnsafeStep(Pos, Player);
					if (CurField.DCaptureCount > 0)
						AddNewTrajectory(CurField, CurField.PointsSeq.end() - (CurrentDepth - Depth), CurField.PointsSeq.end(), Player);
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
						AddNewTrajectory(CurField, CurField.PointsSeq.end() - (CurrentDepth - Depth), CurField.PointsSeq.end(), Player);
					else if (Depth > 0)
						BuildTrajectoriesRecursive(CurField, Depth - 1, Player);

					CurField.UndoStep();
				}
			}
		}
	}
	void BuildCurrentTrajectoriesRecursive(Field &CurField, uint Depth, short Player, uint CheckedPos)
	{
		for (uint Pos = CurField.MinPos; Pos <= CurField.MaxPos; Pos++)
		{
			if (CurField.PuttingAllow(Pos) && CurField.IsNearPoints(Pos, Player))
			{
				if (CurField.DoUnsafeStepAndCheckPoint(Pos, Player, CheckedPos))
					AddNewTrajectory(CurField, CurField.PointsSeq.end() - (CurrentDepth - Depth), CurField.PointsSeq.end(), Player);
				else if (!CurField.IsInEmptyBase(Pos) && Depth > 0)
					BuildCurrentTrajectoriesRecursive(CurField, Depth - 1, Player, CheckedPos);

				CurField.UndoStep();
			}
		}
	}

public:
	// Конструктор.
	inline TrajectoryList()
	{
		Clear();
	}
	// Конструктор копирования.
	inline TrajectoryList(const TrajectoryList &Orig)
	{
		Count = Orig.Count;
		for (uint i = 0; i < Count; i++)
			Trajectories[i].Copy(Orig.Trajectories[i]);
	}

	// Очищает список траекторий.
	inline void Clear()
	{
		Count = 0;
	}

	// Построить траектории на поле CurrentField длиной Depth за игрока Player.
	inline void BuildTrajectories(Field &CurField, uint Depth, short Player)
	{
		CurrentDepth = Depth;
		BuildTrajectoriesRecursive(CurField, Depth - 1, Player);
	}
	// Построить траектории на поле CurrentField на основе уже существующих траекторий LastTrajectories.
	inline void BuildEnemyTrajectories(Field &CurField, TrajectoryList &LastTrajectories, uint Pos, uint Depth)
	{
		for (uint i = 0; i < LastTrajectories.Count; i++)
			if ((LastTrajectories.Trajectories[i].Count <= Depth || (LastTrajectories.Trajectories[i].Count == Depth + 1 && LastTrajectories.Trajectories[i].Find(Pos) != -1)) && IsTrajectoryValid(CurField, LastTrajectories.Trajectories[i], Pos))
				AddNewTrajectory(LastTrajectories.Trajectories[i], Pos);
	}

	inline void BuildCurrentTrajectories(Field &CurField, TrajectoryList &LastTrajectories, uint Pos, uint Depth, short Player)
	{
		for (uint i = 0; i < LastTrajectories.Count; i++)
			if (IsTrajectoryValid(CurField, LastTrajectories.Trajectories[i]))
				AddNewTrajectory(LastTrajectories.Trajectories[i]);

		CurrentDepth = Depth;
		BuildCurrentTrajectoriesRecursive(CurField, Depth - 1, Player, Pos);
	}

	inline void GetPoints(vector<uint> &Points)
	{
		for (uint i = 0; i < Count; i++)
			if (!Trajectories[i].Excluded)
				for (uint j = 0; j < Trajectories[i].Count; j++)
					if (find(Points.begin(), Points.end(), Trajectories[i].Points[j]) == Points.end())
						Points.push_back(Trajectories[i].Points[j]);
	}

	// Проецирует траектории на доску TrajectoriesBoard (для каждой точки Pos очередной траектории инкрементирует TrajectoriesBoard[Pos]).
	// Для оптимизации в данной реализации функции не проверяются траектории на исключенность (поле Excluded).
	inline void Project(uint TrajectoriesBoard[])
	{
		for (uint i = 0; i < Count; i++)
			if (!Trajectories[i].Excluded)
				for (uint j = 0; j < Trajectories[i].Count; j++)
					TrajectoriesBoard[Trajectories[i].Points[j]]++;
	}
	// Удаляет проекцию траекторий с доски TrajectoriesBoard.
	inline void UnProject(uint TrajectoriesBoard[])
	{
		for (uint i = 0; i < Count; i++)
			if (!Trajectories[i].Excluded)
				for (uint j = 0; j < Trajectories[i].Count; j++)
					TrajectoriesBoard[Trajectories[i].Points[j]]--;
	}

	bool ExcludeUnnecessaryTrajectories(uint TrajectoriesBoard[])
	{
		uint c;
		bool NeedExclude = false;

		for (uint i = 0; i < Count; i++)
		{
			if (Trajectories[i].Excluded)
				continue;
			// Считаем в c количество точек, входящих только в эту траекторию.
			c = 0;
			for (uint j = 0; j < Trajectories[i].Count; j++)
				if (TrajectoriesBoard[Trajectories[i].Points[j]] == 1)
					c++;
			// Если точек, входящих только в эту траекторию, > 1, то исключаем эту траекторию.
			if (c > 1)
			{
				NeedExclude = true;
				Trajectories[i].Excluded = true;
				for (uint j = 0; j < Trajectories[i].Count; j++)
					TrajectoriesBoard[Trajectories[i].Points[j]]--;
			}
		}

		return NeedExclude;
	}
	inline void IncludeAllTrajectories()
	{
		for (uint i = 0; i < Count; i++)
			Trajectories[i].Excluded = false;
	}

	// Возвращает хеш Зобриста пересечения двух траекторий.
	inline static ulong GetIntersectHash(Trajectory &T1, Trajectory &T2)
	{
		ulong TempHash = T1.Hash;
		for (uint i = 0; i < T2.Count; i++)
			if (T1.Find(T2.Points[i]) == -1)
				TempHash ^= GetZobristHash(T2.Points[i]);
		return TempHash;
	}

	// Исключает составные траектории.
	inline void ExcludeCompositeTrajectories()
	{
		for (uint k = 0; k < Count; k++)
			for (uint i = 0; i < Count - 1; i++)
				if (Trajectories[k].Count > Trajectories[i].Count)
					for (uint j = i + 1; j < Count; j++)
						if (Trajectories[k].Count > Trajectories[j].Count && Trajectories[k].Hash == GetIntersectHash(Trajectories[i], Trajectories[j]))
							Trajectories[k].Excluded = true;
	}
};