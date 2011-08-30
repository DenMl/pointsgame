#pragma once

#include "BasicTypes.h"
#include "BasicConstants.h"
#include "Zobrist.h"
#include "Field.h"
#include "Config.h"
#include "Trajectory.h"
#include <vector>
#include <algorithm>

using namespace std;

class TrajectoryList
{
private:
	uint CurrentDepth;

public:
	vector<Trajectory> Trajectories;

private:
	template<typename _InIt>
	inline void AddNewTrajectory(Field &CurrentField, _InIt begin, _InIt end, player Player)
	{
		size_t TempHash = 0;

		// Эвристические проверки.
		// Каждая точка траектории должна окружать что-либо и иметь рядом хотя бы 2 группы точек.
		// Если нет - не добавляем эту траекторию.
		for (auto i = begin; i < end; i++)
			if (!CurrentField.IsBaseBound(*i) || (CurrentField.NumberNearGroups(*i, Player) < 2))
				return;

		// Высчитываем хеш траектории и сравниваем с уже существующими для исключения повторов.
		for (auto i = begin; i < end; i++)
			TempHash ^= GetZobristHash(*i);
		for (auto i = Trajectories.begin(); i < Trajectories.end(); i++)
			if (TempHash == i->hash())
				return; // В теории возможны коллизии. Неплохо было бы сделать точную проверку.

		Trajectories.resize(Trajectories.size() + 1);

		for (auto i = begin; i < end; i++)
		{
			// Добавляем точку в PointsSeq траектории.
			Trajectories.back().push_back(*i);
		}
	}
	inline void AddNewTrajectory(const Trajectory &CurTrajectory)
	{
		Trajectories.resize(Trajectories.size() + 1);
		Trajectories.back().Copy(CurTrajectory);
	}
	// Добавляет траекторию, полученную из CurTrajectory исключением из нее точки Pos.
	// Также контролирует недобавление нулевых траекторий.
	inline void AddNewTrajectory(const Trajectory &CurTrajectory, const pos Pos)
	{
		if (CurTrajectory.size() == 1)
			return;

		Trajectories.resize(Trajectories.size() + 1);
		Trajectories.back().Copy(CurTrajectory, Pos);
	}

	// Проверяет, во все ли точки траектории можно сделать ход, кроме, возможно, точки Pos.
	inline static bool IsTrajectoryValid(Field &CurField, const Trajectory &CurTrajectory, pos Pos)
	{
		for (auto i = CurTrajectory.begin(); i < CurTrajectory.end(); i++)
			if (*i != Pos && !CurField.PuttingAllow(*i))
				return false;
		return true;
	}
	inline static bool IsTrajectoryValid(Field &CurField, const Trajectory &CurTrajectory)
	{
		for (auto i = CurTrajectory.begin(); i < CurTrajectory.end(); i++)
			if (!CurField.PuttingAllow(*i))
				return false;
		return true;
	}

	// Рекурсивная функция построения траекторий.
	void BuildTrajectoriesRecursive(Field &CurField, uint Depth, player Player)
	{
		for (pos Pos = CurField.MinPos; Pos <= CurField.MaxPos; Pos++)
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
	void BuildCurrentTrajectoriesRecursive(Field &CurField, uint Depth, player Player, pos CheckedPos)
	{
		for (pos Pos = CurField.MinPos; Pos <= CurField.MaxPos; Pos++)
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
		Trajectories.reserve(MAX_TRAJECTORIES_COUNT);
	}
	// Конструктор копирования.
// 	inline TrajectoryList(const TrajectoryList &Orig)
// 	{
// 		Count = Orig.Count;
// 		for (uint i = 0; i < Count; i++)
// 			Trajectories[i].Copy(Orig.Trajectories[i]);
// 	}

	// Очищает список траекторий.
	inline void Clear()
	{
		Trajectories.clear();
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
		for (auto i = LastTrajectories.Trajectories.begin(); i < LastTrajectories.Trajectories.end(); i++)
			if ((i->size() <= Depth || (i->size() == Depth + 1 && find(i->begin(), i->end(), Pos) != i->end())) && IsTrajectoryValid(CurField, *i, Pos))
				AddNewTrajectory(*i, Pos);
	}

	inline void BuildCurrentTrajectories(Field &CurField, TrajectoryList &LastTrajectories, uint Pos, uint Depth, short Player)
	{
		for (auto i = LastTrajectories.Trajectories.begin(); i < LastTrajectories.Trajectories.end(); i++)
			if (IsTrajectoryValid(CurField, *i))
				AddNewTrajectory(*i);

		CurrentDepth = Depth;
		BuildCurrentTrajectoriesRecursive(CurField, Depth - 1, Player, Pos);
	}

	inline void GetPoints(static_vector<pos, MAX_CHAIN_POINTS> &Points)
	{
		for (auto i = Trajectories.begin(); i < Trajectories.end(); i++)
			if (!i->excluded())
				for (auto j = i->begin(); j < i->end(); j++)
					if (find(Points.begin(), Points.end(), *j) == Points.end())
						Points.push_back(*j);
	}

	// Проецирует траектории на доску TrajectoriesBoard (для каждой точки Pos очередной траектории инкрементирует TrajectoriesBoard[Pos]).
	// Для оптимизации в данной реализации функции не проверяются траектории на исключенность (поле Excluded).
	inline void Project(uint TrajectoriesBoard[])
	{
		for (auto i = Trajectories.begin(); i < Trajectories.end(); i++)
			if (!i->excluded())
				for (auto j = i->begin(); j < i->end(); j++)
					TrajectoriesBoard[*j]++;
	}
	// Удаляет проекцию траекторий с доски TrajectoriesBoard.
	inline void UnProject(uint TrajectoriesBoard[])
	{
		for (auto i = Trajectories.begin(); i < Trajectories.end(); i++)
			if (!i->excluded())
				for (auto j = i->begin(); j < i->end(); j++)
					TrajectoriesBoard[*j]--;
	}

	bool ExcludeUnnecessaryTrajectories(uint TrajectoriesBoard[])
	{
		uint c;
		bool NeedExclude = false;

		for (auto i = Trajectories.begin(); i < Trajectories.end(); i++)
		{
			if (i->excluded())
				continue;
			// Считаем в c количество точек, входящих только в эту траекторию.
			c = 0;
			for (auto j = i->begin(); j < i->end(); j++)
				if (TrajectoriesBoard[*j] == 1)
					c++;
			// Если точек, входящих только в эту траекторию, > 1, то исключаем эту траекторию.
			if (c > 1)
			{
				NeedExclude = true;
				i->exclude();
				for (auto j = i->begin(); j < i->end(); j++)
					TrajectoriesBoard[*j]--;
			}
		}

		return NeedExclude;
	}
	inline void IncludeAllTrajectories()
	{
		for (auto i = Trajectories.begin(); i < Trajectories.end(); i++)
			i->include();
	}

	// Возвращает хеш Зобриста пересечения двух траекторий.
	inline static ulong GetIntersectHash(Trajectory &T1, Trajectory &T2)
	{
		ulong TempHash = T1.hash();
		for (auto i = T2.begin(); i < T2.end(); i++)
			if (find(T1.begin(), T1.end(), *i) == T1.end())
				TempHash ^= GetZobristHash(*i);
		return TempHash;
	}

	// Исключает составные траектории.
	inline void ExcludeCompositeTrajectories()
	{
		for (auto k = Trajectories.begin(); k < Trajectories.end(); k++)
			for (auto i = Trajectories.begin(); i < Trajectories.end() - 1; i++)
				if (k->size() > i->size())
					for (auto j = i + 1; j < Trajectories.end(); j++)
						if (k->size() > j->size() && k->hash() == GetIntersectHash(*i, *j))
							k->exclude();
	}
};