#include "Config.h"
#include "MinMaxEstimate.h"
#include "Field.h"
#include "TrajectoryList.h"
#include <omp.h>

// inc[] - массив, в который заносятся инкременты.
// size - размерность этого массива.
_int increment(_int inc[], _int size)
{
	_int p1 = 1, p2 = 1, p3 = 1, s = -1;

	// Заполняем массив элементов по формуле Роберта Седжвика.
	do
	{
		if (++s % 2)
		{
			inc[s] = 8*p1 - 6*p2 + 1;
		}
		else
		{
			inc[s] = 9*p1 - 9*p3 + 1;
			p2 *= 2;
			p3 *= 2;
		}
		p1 *= 2;
	} while (3*inc[s] < size); // Заполняем массив, пока текущая инкремента хотя бы в 3 раза меньше количества элементов в массиве.

	// Возвращаем количество элементов в массиве.
	return s > 0 ? --s : 0;
}
void ShellSort(_int a[], _int values[], _int size)
{
	// inc - инкремент, расстояние между элементами сравнения.
	// i и j - стандартные переменные цикла.
	// seq[40] - массив, в котором хранятся инкременты.
	_int inc, j, seq[40];

	// Вычисление последовательности приращений.
	_int s = increment(seq, size);
	while (s >= 0)
	{
		// Извлекаем из массива очередную инкременту.
		inc = seq[s--];

		// Сортировка вставками с инкрементами inc[].
		for (_int i = inc; i < size; i++)
		{
			_int temp = a[i];
			// Сдвигаем элементы до тех пор, пока не дойдем до конца или не упорядочим в нужном порядке.
			for (j = i-inc; (j >= 0) && (values[a[j]] < values[temp]); j -= inc)
				a[j+inc] = a[j];
			// После всех сдвигов ставим на место j+inc элемент, который находился на i месте.
			a[j+inc] = temp;
		}
	}
}

inline void GetPoints(TrajectoryList &Trajectories1, TrajectoryList &Trajectories2, _int TrajectoriesBoard[], GameStack<_int, MAX_CHAIN_POINTS> &Moves)
{
	Trajectories1.ExcludeCompositeTrajectories();
	Trajectories2.ExcludeCompositeTrajectories();

	// Проецируем неисключенные траектории на доску.
	Trajectories1.Project(TrajectoriesBoard);
	Trajectories2.Project(TrajectoriesBoard);
	// Исключаем те траектории, у которых имеется более одной точки, принадлежащей только ей.
	while (Trajectories1.ExcludeUnnecessaryTrajectories(TrajectoriesBoard) || Trajectories2.ExcludeUnnecessaryTrajectories(TrajectoriesBoard));
	// Получаем список точек, входящих в оставшиеся неисключенные траектории.
	Moves.Clear();
	Trajectories1.GetPoints(Moves);
	Trajectories2.GetPoints(Moves);

#if ALPHABETA_SORT
	// Сортируем точки по убыванию количества траекторий, в которые они входят.
	ShellSort(Moves.Stack, TrajectoriesBoard, Moves.Count);
#endif

	// Очищаем доску от проекций.
	Trajectories1.UnProject(TrajectoriesBoard);
	Trajectories2.UnProject(TrajectoriesBoard);
	// После получения списка ходов обратно включаем в рассмотрение все траектории (для следующего уровня рекурсии).
	Trajectories1.IncludeAllTrajectories();
	Trajectories2.IncludeAllTrajectories(); //TODO: MAX_SPEED
}

// Рекурсивная функция минимакса.
// CurField - поле, на котором ведется поиск лучшего хода.
// TrajectoriesBoard - доска, на которую проецируются траектории. Должна быть заполнена нулями. Нужна для оптимизации.
// Depth - глубина просчета.
// Pos - последний выбранный, но не сделанный ход.
// alpha, beta - интервал оценок, вне которого искать нет смысла.
_int Negamax(Field &CurField, _int TrajectoriesBoard[], _int Depth, _int Pos, TrajectoryList &LastEnemyTrajectories, TrajectoryList &LastCurrentTrajectories, _int alpha, _int beta)
{
	_int BestEstimate = -Infinity;
	TrajectoryList Trajectories[2];

	// Делаем ход, выбранный на предыдущем уровне рекурсии, после чего этот ход становится вражеским.
	CurField.DoUnsafeStep(Pos);

	if (Depth == 0)
	{
		BestEstimate = -(CurField.CaptureCount[CurField.CurPlayer] - CurField.CaptureCount[CurField.EnemyPlayer]);
		CurField.UndoStep();
		return BestEstimate;
	}

	if (CurField.DCaptureCount < 0) // Если точка поставлена в окружение.
	{
		CurField.UndoStep();
		return -(Infinity); // Для CurPlayer это хорошо, то есть оценка Infinity.
	}

	if (Depth > 1)
		Trajectories[CurField.EnemyPlayer].BuildEnemyTrajectories(CurField, LastEnemyTrajectories, Pos, Depth / 2);
	Trajectories[CurField.CurPlayer].BuildCurrentTrajectories(CurField, LastCurrentTrajectories, Pos, (Depth + 1) / 2, CurField.CurPlayer);
	//Trajectories[CurField.CurPlayer].BuildTrajectories(CurField, (Depth + 1) / 2, CurField.CurPlayer);
	
	GameStack<_int, MAX_CHAIN_POINTS> Moves;
	GetPoints(Trajectories[0], Trajectories[1], TrajectoriesBoard, Moves);

	for (_int i = 0; i < Moves.Count; i++)
	{
		_int CurEstimate = Negamax(CurField, TrajectoriesBoard, Depth - 1, Moves.Stack[i], Trajectories[CurField.CurPlayer], Trajectories[CurField.EnemyPlayer], -beta, -alpha);
		if (CurEstimate > BestEstimate)
		{
			BestEstimate = CurEstimate;
			if (BestEstimate >= beta)
				break;
			if (BestEstimate > alpha)
				alpha = BestEstimate;
		}
	}
	if (BestEstimate == -Infinity)
		BestEstimate = CurField.CaptureCount[CurField.CurPlayer] - CurField.CaptureCount[CurField.EnemyPlayer];

	CurField.UndoStep();

	return -(BestEstimate);
}

_int GetEnemyEstimate(Field &CurrentField, TrajectoryList &CurrentTrajectories, TrajectoryList &EnemyTrajectories, _int TrajectoriesBoard[], _int Depth)
{
	TrajectoryList TempTrajectories;
	GameStack<_int, MAX_CHAIN_POINTS> Moves;
	_int Result = 0;

	TempTrajectories.BuildEnemyTrajectories(CurrentField, CurrentTrajectories, 0, (Depth + 1) / 2 - 1);
	GetPoints(EnemyTrajectories, TempTrajectories, TrajectoriesBoard, Moves);

	CurrentField.SetNextPlayer();
	Depth -= 2;
	omp_lock_t lock;
	omp_init_lock(&lock);
	#pragma omp parallel
	{
		_int MaxScore = 0, CurrentScore;
		Field* LocalField = new Field(CurrentField);
		_int TrajectoriesBoard[PointsLength22] = {0};

		#pragma omp for schedule(dynamic, 1)
		for (_int i = 0; i < Moves.Count; i++)
		{
			CurrentScore = Negamax(*LocalField, TrajectoriesBoard, Depth, Moves.Stack[i], EnemyTrajectories, CurrentTrajectories, -Infinity, Infinity);
			if (CurrentScore > MaxScore)
				MaxScore = CurrentScore;
		}

		delete LocalField;

		omp_set_lock(&lock);
		if (MaxScore > Result)
			Result = MaxScore;
		omp_unset_lock(&lock);
	}
	omp_destroy_lock(&lock);
	CurrentField.SetNextPlayer();

	EnemyTrajectories.IncludeAllTrajectories();

	return -Result;
}

// MainField - поле, на котором производится оценка.
// Depth - глубина оценки.
_int MinMaxEstimate(Field &MainField, _int Depth, GameStack<_int, MAX_CHAIN_POINTS> &Moves)
{
	// Минимально и максимально возможная оценка.
	_int Result = -Infinity;
	// Главные траектории - свои и вражеские.
	TrajectoryList Trajectories[2];
	// Доска, на которую идет проецирование траекторий. Необходима для оптимизации работы с памятью.
	_int TrajectoriesBoard[PointsLength22] = {0};
	GameStack<_int, MAX_CHAIN_POINTS> PotentialMoves, OutMoves;

	// Делаем что-то только когда глубина просчета положительная.
	if (Depth <= 0)
		return 0;

	// Строим свои траектоии.
	Trajectories[MainField.CurPlayer].BuildTrajectories(MainField, (Depth + 1) / 2, MainField.CurPlayer);
	// Строим траектории противника.
	if (Depth > 1)
		Trajectories[MainField.EnemyPlayer].BuildTrajectories(MainField, Depth / 2, MainField.EnemyPlayer);

	GetPoints(Trajectories[0], Trajectories[1], TrajectoriesBoard, PotentialMoves);
	PotentialMoves.Intersect(Moves);

	if (PotentialMoves.Count == 0)
		return 0;

	_int alpha = -Infinity + 1;
	omp_lock_t lock;
	omp_init_lock(&lock);
	#pragma omp parallel
	{
		Field* LocalField = new Field(MainField);
		_int TrajectoriesBoard[PointsLength22] = {0};
		_int BestEstimate = -Infinity;
		GameStack<_int, MAX_CHAIN_POINTS> BestMoves;

		#pragma omp for schedule(dynamic, 1)
		for (_int i = 0; i < PotentialMoves.Count; i++)
		{
			_int CurEstimate = Negamax(*LocalField, TrajectoriesBoard, Depth - 1, PotentialMoves.Stack[i], Trajectories[LocalField->CurPlayer], Trajectories[LocalField->EnemyPlayer], -Infinity, -alpha + 1);
			if (CurEstimate > alpha) // Обновляем нижнюю границу.
				alpha = CurEstimate;
			if (CurEstimate > BestEstimate)
			{
				BestEstimate = CurEstimate;
				BestMoves.Clear();
				BestMoves.Push(PotentialMoves.Stack[i]);
			}
			else if (CurEstimate == BestEstimate)
			{
				BestMoves.Push(PotentialMoves.Stack[i]);
			}
		}

		omp_set_lock(&lock);
		if (BestEstimate > Result)
		{
			Result = BestEstimate;
			OutMoves.Copy(BestMoves);
		}
		else if (BestEstimate == Result)
		{
			OutMoves.Union(BestMoves);
		}
		omp_unset_lock(&lock);

		delete LocalField;
	}
	omp_destroy_lock(&lock);

	if (Result > GetEnemyEstimate(MainField, Trajectories[MainField.CurPlayer], Trajectories[MainField.EnemyPlayer], TrajectoriesBoard, Depth))
		Moves.Copy(OutMoves);
	return Result;
}