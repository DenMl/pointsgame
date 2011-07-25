#pragma once

#include "BasicTypes.h"
#include "BasicConstants.h"
#include "Zobrist.h"
#include "Field.h"
#include "GameStack.h"
#include <assert.h>
#include "Config.h"

const p_int MAX_TRAJECTORY_LENGTH = 8;
const p_int MAX_TRAJECTORIES_COUNT = 256;

struct Trajectory
{
	p_int Points[MAX_TRAJECTORY_LENGTH];
	p_int Count;

	p_int Hash;

	bool Excluded;

	inline void Clear()
	{
		Count = 0;
		Hash = 0;
		Excluded = false;
	}

	// Добавляет точку в траекторию.
	inline void Push(p_int Pos)
	{
		Points[Count] = Pos;
		Count++;
		Hash ^= GetZobristHash(Pos);
	}
	// Добавляет точку в траекторию, не обновляя хеш.
	inline void FastPush(p_int Pos)
	{
		Points[Count] = Pos;
		Count++;
	}

	inline void Copy(const Trajectory &Orig)
	{
		for (p_int i = 0; i < Orig.Count; i++)
			Points[i] = Orig.Points[i];
		Count = Orig.Count;
		Hash = Orig.Hash;
		Excluded = Orig.Excluded;
	}
	inline void Copy(const Trajectory &Orig, const p_int Pos)
	{
		Count = 0;
		for (p_int i = 0; i < Orig.Count; i++)
			if (Orig.Points[i] != Pos)
				FastPush(Orig.Points[i]);
		Hash = Orig.Hash ^ GetZobristHash(Pos);
		Excluded = Orig.Excluded;
	}

	inline const p_int Find(const p_int Pos)
	{
		for (p_int i = 0; i < Count; i++)
			if (Points[i] == Pos)
				return i;
		return -1;
	}
};

class TrajectoryList
{
private:
	p_int CurrentDepth;

public:
	Trajectory Trajectories[MAX_TRAJECTORIES_COUNT];
	p_int Count;

private:
	inline void AddNewTrajectory(Field &CurrentField, const p_int Points[], p_int Length, p_int Player)
	{
		p_int TempHash = 0;

		// Эвристические проверки.
		// Каждая точка траектории должна окружать что-либо и иметь рядом хотя бы 2 группы точек.
		// Если нет - не добавляем эту траекторию.
		for (p_int i = 0; i < Length; i++)
			if (!CurrentField.IsBaseBound(Points[i]) || (CurrentField.NumberNearGroups(Points[i], Player) < 2))
				return;

		// Высчитываем хеш траектории и сравниваем с уже существующими для исключения повторов.
		for (p_int i = 0; i < Length; i++)
			TempHash ^= GetZobristHash(Points[i]);
		for (p_int i = 0; i < Count; i++)
			if (TempHash == Trajectories[i].Hash)
				return; // В теории возможны коллизии. Неплохо было бы сделать точную проверку.

		Trajectories[Count].Excluded = false;
		// Запоминаем хеш новой траектории.
		Trajectories[Count].Hash = TempHash;

		for (p_int i = 0; i < Length; i++)
		{
			// Добавляем точку в PointsSeq траектории.
			Trajectories[Count].Points[i] = Points[i];
		}
		// Запоминаем количество точек в траектории.
		Trajectories[Count].Count = Length;
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
	inline void AddNewTrajectory(const Trajectory &CurTrajectory, const p_int Pos)
	{
		if (CurTrajectory.Count == 1)
			return;

		Trajectories[Count].Copy(CurTrajectory, Pos);
		Count++;
	}

	// Проверяет, во все ли точки траектории можно сделать ход, кроме, возможно, точки Pos.
	inline static bool IsTrajectoryValid(Field &CurField, const Trajectory &CurTrajectory, p_int Pos)
	{
		for (p_int i = 0; i < CurTrajectory.Count; i++)
			if (CurTrajectory.Points[i] != Pos && !CurField.PuttingAllow(CurTrajectory.Points[i]))
				return false;
		return true;
	}
	inline static bool IsTrajectoryValid(Field &CurField, const Trajectory &CurTrajectory)
	{
		for (p_int i = 0; i < CurTrajectory.Count; i++)
			if (!CurField.PuttingAllow(CurTrajectory.Points[i]))
				return false;
		return true;
	}

	// Рекурсивная функция построения траекторий.
	void BuildTrajectoriesRecursive(Field &CurField, p_int Depth, p_int Player)
	{
		for (p_int Pos = CurField.MinPos; Pos <= CurField.MaxPos; Pos++)
		{
			if (CurField.PuttingAllow(Pos) && CurField.IsNearPoints(Pos, Player))
			{
				if (CurField.IsInEmptyBase(Pos)) // Если поставили в пустую базу (свою или нет), то дальше строить траекторию нет нужды.
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
	void BuildCurrentTrajectoriesRecursive(Field &CurField, p_int Depth, p_int Player, p_int CheckedPos)
	{
		for (p_int Pos = CurField.MinPos; Pos <= CurField.MaxPos; Pos++)
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
	// Конструктор.
	inline TrajectoryList()
	{
		Clear();
	}
	// Конструктор копирования.
	inline TrajectoryList(const TrajectoryList &Orig)
	{
		Count = Orig.Count;
		for (p_int i = 0; i < Count; i++)
			Trajectories[i].Copy(Orig.Trajectories[i]);
	}

	// Очищает список траекторий.
	inline void Clear()
	{
		Count = 0;
	}

	// Построить траектории на поле CurrentField длиной Depth за игрока Player.
	inline void BuildTrajectories(Field &CurField, p_int Depth, p_int Player)
	{
		CurrentDepth = Depth;
		BuildTrajectoriesRecursive(CurField, Depth - 1, Player);
	}
	// Построить траектории на поле CurrentField на основе уже существующих траекторий LastTrajectories.
	inline void BuildEnemyTrajectories(Field &CurField, TrajectoryList &LastTrajectories, p_int Pos, p_int Depth)
	{
		for (p_int i = 0; i < LastTrajectories.Count; i++)
			if ((LastTrajectories.Trajectories[i].Count <= Depth || (LastTrajectories.Trajectories[i].Count == Depth + 1 && LastTrajectories.Trajectories[i].Find(Pos) != -1)) && IsTrajectoryValid(CurField, LastTrajectories.Trajectories[i], Pos))
				AddNewTrajectory(LastTrajectories.Trajectories[i], Pos);
	}

	inline void BuildCurrentTrajectories(Field &CurField, TrajectoryList &LastTrajectories, p_int Pos, p_int Depth, p_int Player)
	{
		for (p_int i = 0; i < LastTrajectories.Count; i++)
			if (IsTrajectoryValid(CurField, LastTrajectories.Trajectories[i]))
				AddNewTrajectory(LastTrajectories.Trajectories[i]);

		CurrentDepth = Depth;
		BuildCurrentTrajectoriesRecursive(CurField, Depth - 1, Player, Pos);
	}

	inline void GetPoints(GameStack<p_int, MAX_CHAIN_POINTS> &Points)
	{
		for (p_int i = 0; i < Count; i++)
			if (!Trajectories[i].Excluded)
				for (p_int j = 0; j < Trajectories[i].Count; j++)
					if (Points.Find(Trajectories[i].Points[j]) == -1)
						Points.Push(Trajectories[i].Points[j]);
	}

	// Проецирует траектории на доску TrajectoriesBoard (для каждой точки Pos очередной траектории инкрементирует TrajectoriesBoard[Pos]).
	// Для оптимизации в данной реализации функции не проверяются траектории на исключенность (поле Excluded).
	inline void Project(p_int TrajectoriesBoard[])
	{
		for (p_int i = 0; i < Count; i++)
			if (!Trajectories[i].Excluded)
				for (p_int j = 0; j < Trajectories[i].Count; j++)
					TrajectoriesBoard[Trajectories[i].Points[j]]++;
	}
	// Удаляет проекцию траекторий с доски TrajectoriesBoard.
	inline void UnProject(p_int TrajectoriesBoard[])
	{
		for (p_int i = 0; i < Count; i++)
			if (!Trajectories[i].Excluded)
				for (p_int j = 0; j < Trajectories[i].Count; j++)
					TrajectoriesBoard[Trajectories[i].Points[j]]--;
	}

	bool ExcludeUnnecessaryTrajectories(p_int TrajectoriesBoard[])
	{
		p_int c;
		bool NeedExclude = false;

		for (p_int i = 0; i < Count; i++)
		{
			if (Trajectories[i].Excluded)
				continue;
			// Считаем в c количество точек, входящих только в эту траекторию.
			c = 0;
			for (p_int j = 0; j < Trajectories[i].Count; j++)
				if (TrajectoriesBoard[Trajectories[i].Points[j]] == 1)
					c++;
			// Если точек, входящих только в эту траекторию, > 1, то исключаем эту траекторию.
			if (c > 1)
			{
				NeedExclude = true;
				Trajectories[i].Excluded = true;
				for (p_int j = 0; j < Trajectories[i].Count; j++)
					TrajectoriesBoard[Trajectories[i].Points[j]]--;
			}
		}

		return NeedExclude;
	}
	inline void IncludeAllTrajectories()
	{
		for (p_int i = 0; i < Count; i++)
			Trajectories[i].Excluded = false;
	}

	// Возвращает хеш Зобриста пересечения двух траекторий.
	inline static p_int GetIntersectHash(Trajectory &T1, Trajectory &T2)
	{
		p_int TempHash = T1.Hash;
		for (p_int i = 0; i < T2.Count; i++)
			if (T1.Find(T2.Points[i]) == -1)
				TempHash ^= GetZobristHash(T2.Points[i]);
		return TempHash;
	}

	// Исключает составные траектории.
	inline void ExcludeCompositeTrajectories()
	{
		for (p_int k = 0; k < Count; k++)
			for (p_int i = 0; i < Count - 1; i++)
				if (Trajectories[k].Count > Trajectories[i].Count)
					for (p_int j = i + 1; j < Count; j++)
						if (Trajectories[k].Count > Trajectories[j].Count && Trajectories[k].Hash == GetIntersectHash(Trajectories[i], Trajectories[j]))
							Trajectories[k].Excluded = true;
	}
};