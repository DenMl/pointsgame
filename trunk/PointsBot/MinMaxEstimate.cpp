#include "Config.h"
#include "MinMaxEstimate.h"
#include "Field.h"
#include "TrajectoryList.h"
#include <omp.h>
#include <algorithm>
#include <vector>

using namespace std;

// // inc[] - массив, в который заносятся инкременты.
// // size - размерность этого массива.
// p_int increment(p_int inc[], p_int size)
// {
// 	p_int p1 = 1, p2 = 1, p3 = 1, s = -1;
// 
// 	// Заполняем массив элементов по формуле Роберта Седжвика.
// 	do
// 	{
// 		if (++s % 2)
// 		{
// 			inc[s] = 8*p1 - 6*p2 + 1;
// 		}
// 		else
// 		{
// 			inc[s] = 9*p1 - 9*p3 + 1;
// 			p2 *= 2;
// 			p3 *= 2;
// 		}
// 		p1 *= 2;
// 	} while (3*inc[s] < size); // Заполняем массив, пока текущая инкремента хотя бы в 3 раза меньше количества элементов в массиве.
// 
// 	// Возвращаем количество элементов в массиве.
// 	return s > 0 ? --s : 0;
// }
// void ShellSort(p_int a[], p_int values[], p_int size)
// {
// 	// inc - инкремент, расстояние между элементами сравнения.
// 	// i и j - стандартные переменные цикла.
// 	// seq[40] - массив, в котором хранятся инкременты.
// 	p_int inc, j, seq[40];
// 
// 	// Вычисление последовательности приращений.
// 	p_int s = increment(seq, size);
// 	while (s >= 0)
// 	{
// 		// Извлекаем из массива очередную инкременту.
// 		inc = seq[s--];
// 
// 		// Сортировка вставками с инкрементами inc[].
// 		for (p_int i = inc; i < size; i++)
// 		{
// 			p_int temp = a[i];
// 			// Сдвигаем элементы до тех пор, пока не дойдем до конца или не упорядочим в нужном порядке.
// 			for (j = i-inc; (j >= 0) && (values[a[j]] < values[temp]); j -= inc)
// 				a[j+inc] = a[j];
// 			// После всех сдвигов ставим на место j+inc элемент, который находился на i месте.
// 			a[j+inc] = temp;
// 		}
// 	}
// }

struct MovesComparer
{
	uint *TrajectoriesBoard;

	MovesComparer(uint *Board)
	{
		TrajectoriesBoard = Board;
	}

	bool operator() (int i, int j)
	{
		return (TrajectoriesBoard[i] < TrajectoriesBoard[j]);
	}
};

inline void GetPoints(TrajectoryList &Trajectories1, TrajectoryList &Trajectories2, uint TrajectoriesBoard[], GameStack<uint, MAX_CHAIN_POINTS> &Moves)
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
	//ShellSort(Moves.Stack, TrajectoriesBoard, Moves.Count);
	vector<uint> v(Moves.Stack, Moves.Stack + Moves.Count);
	MovesComparer mc (TrajectoriesBoard);
	sort(v.begin(), v.end(), mc); //TODO: ПРОВЕРИТЬ!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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
// На выходе оценка позиции для CurPlayer (до хода Pos).
int Negamax(Field &CurField, uint TrajectoriesBoard[], uint Depth, uint Pos, TrajectoryList &LastEnemyTrajectories, TrajectoryList &LastCurrentTrajectories, int alpha, int beta)
{
	int BestEstimate = INT_MIN + 1;
	TrajectoryList Trajectories[2];

	// Делаем ход, выбранный на предыдущем уровне рекурсии, после чего этот ход становится вражеским.
	CurField.DoUnsafeStep(Pos);

	if (Depth == 0)
	{
		BestEstimate = CurField.GetScore(CurField.CurPlayer);
		CurField.UndoStep();
		return -BestEstimate;
	}

	if (CurField.DCaptureCount < 0) // Если точка поставлена в окружение.
	{
		CurField.UndoStep();
		return INT_MIN + 1; // Для CurPlayer это хорошо, то есть оценка Infinity.
	}

	if (Depth > 1)
		Trajectories[CurField.EnemyPlayer].BuildEnemyTrajectories(CurField, LastEnemyTrajectories, Pos, Depth / 2);
	Trajectories[CurField.CurPlayer].BuildCurrentTrajectories(CurField, LastCurrentTrajectories, Pos, (Depth + 1) / 2, CurField.CurPlayer);
	//Trajectories[CurField.CurPlayer].BuildTrajectories(CurField, (Depth + 1) / 2, CurField.CurPlayer);
	
	GameStack<uint, MAX_CHAIN_POINTS> Moves;
	GetPoints(Trajectories[0], Trajectories[1], TrajectoriesBoard, Moves);

	for (uint i = 0; i < Moves.Count; i++)
	{
		int CurEstimate = Negamax(CurField, TrajectoriesBoard, Depth - 1, Moves.Stack[i], Trajectories[CurField.CurPlayer], Trajectories[CurField.EnemyPlayer], -beta, -alpha);
		if (CurEstimate > BestEstimate)
		{
			BestEstimate = CurEstimate;
			if (BestEstimate >= beta)
				break;
			if (BestEstimate > alpha)
				alpha = BestEstimate;
		}
	}
	if (BestEstimate == INT_MIN + 1)
		BestEstimate = CurField.GetScore(CurField.CurPlayer);

	CurField.UndoStep();

	return -BestEstimate;
}

int GetEnemyEstimate(Field &CurField, TrajectoryList &CurrentTrajectories, TrajectoryList &EnemyTrajectories, uint TrajectoriesBoard[], uint Depth)
{
	TrajectoryList TempTrajectories;
	GameStack<uint, MAX_CHAIN_POINTS> Moves;
	int Result = INT_MIN + 1;

	TempTrajectories.BuildEnemyTrajectories(CurField, CurrentTrajectories, 0, (Depth + 1) / 2 - 1);
	GetPoints(EnemyTrajectories, TempTrajectories, TrajectoriesBoard, Moves);

	int alpha = INT_MIN + 1;
	CurField.SetNextPlayer();
	Depth -= 2;
	omp_lock_t lock;
	omp_init_lock(&lock);
	#pragma omp parallel
	{
		int MaxEstimate = INT_MIN + 1, CurEstimate;
		Field *LocalField = new Field(CurField);
		uint TrajectoriesBoard[PointsLength22] = {0};

		#pragma omp for schedule(dynamic, 1)
		for (uint i = 0; i < Moves.Count; i++)
		{
			CurEstimate = Negamax(*LocalField, TrajectoriesBoard, Depth, Moves.Stack[i], EnemyTrajectories, CurrentTrajectories, INT_MIN + 1, -alpha);
			if (CurEstimate > alpha) // Обновляем нижнюю границу.
				alpha = CurEstimate;
			if (CurEstimate > MaxEstimate)
				MaxEstimate = CurEstimate;
		}

		delete LocalField;

		omp_set_lock(&lock);
		if (MaxEstimate > Result)
			Result = MaxEstimate;
		omp_unset_lock(&lock);
	}
	omp_destroy_lock(&lock);
	CurField.SetNextPlayer();

	EnemyTrajectories.IncludeAllTrajectories();

	return -Result;
}

// CurField - поле, на котором производится оценка.
// Depth - глубина оценки.
// Moves - на входе возможные ходы, на выходе лучшие из них.
int MinMaxEstimate(Field &CurField, uint Depth, GameStack<uint, MAX_CHAIN_POINTS> &Moves)
{
	// Минимально и максимально возможная оценка.
	int BestEstimate = INT_MIN + 1;
	// Главные траектории - свои и вражеские.
	TrajectoryList Trajectories[2];
	// Доска, на которую идет проецирование траекторий. Необходима для оптимизации работы с памятью.
	uint TrajectoriesBoard[PointsLength22] = {0};
	GameStack<uint, MAX_CHAIN_POINTS> PotentialMoves, OutMoves;
	// Доска с оценками ходов.
	int ScoreBoard[PointsLength22];

	// Делаем что-то только когда глубина просчета положительная и колическтво возможных ходов на входе не равно 0.
	if (Depth <= 0 || Moves.Count == 0)
		return 0;
	// Строим свои траектоии.
	Trajectories[CurField.CurPlayer].BuildTrajectories(CurField, (Depth + 1) / 2, CurField.CurPlayer);
	// Строим траектории противника.
	if (Depth > 1)
		Trajectories[CurField.EnemyPlayer].BuildTrajectories(CurField, Depth / 2, CurField.EnemyPlayer);
	// Получаем ходы из траекторий (которые имеет смысл рассматривать), и находим пересечение со входными возможными точками.
	GetPoints(Trajectories[0], Trajectories[1], TrajectoriesBoard, PotentialMoves);
	PotentialMoves.Intersect(Moves);
	// Если нет возможных ходов, входящих в траектории - выходим.
	if (PotentialMoves.Count == 0)
		return 0;
	// Для почти всех возможных точек, не входящих в траектории оценка будет такая же, как если бы игрок CurPlayer пропустил ход. Записываем оценку для всех ходов, так как потом для ходов, которые входят в траектории она перезапишется.
	int EnemyEstimate = GetEnemyEstimate(CurField, Trajectories[CurField.CurPlayer], Trajectories[CurField.EnemyPlayer], TrajectoriesBoard, Depth);
	for (uint i = 0; i < Moves.Count; i++)
		ScoreBoard[Moves.Stack[i]] = EnemyEstimate;

	int alpha = INT_MIN + 2;
	#pragma omp parallel
	{
		Field* LocalField = new Field(CurField);
		uint TrajectoriesBoard[PointsLength22] = {0};

		#pragma omp for schedule(dynamic, 1)
		for (uint i = 0; i < PotentialMoves.Count; i++)
		{
			int CurEstimate = Negamax(*LocalField, TrajectoriesBoard, Depth - 1, PotentialMoves.Stack[i], Trajectories[LocalField->CurPlayer], Trajectories[LocalField->EnemyPlayer], INT_MIN + 1, -alpha + 1);
			if (CurEstimate > alpha) // Обновляем нижнюю границу.
				alpha = CurEstimate;
			ScoreBoard[PotentialMoves.Stack[i]] = CurEstimate;
		}

		delete LocalField;
	}

	for (uint i = 0; i < Moves.Count; i++)
		if (ScoreBoard[Moves.Stack[i]] > BestEstimate)
		{
			BestEstimate = ScoreBoard[Moves.Stack[i]];
			OutMoves.Clear();
			OutMoves.Push(Moves.Stack[i]);
		}
		else if (ScoreBoard[Moves.Stack[i]] == BestEstimate)
		{
			OutMoves.Push(Moves.Stack[i]);
		}
	Moves.Copy(OutMoves);
	return BestEstimate;
}