#include "Config.h"
#include "MinMaxEstimate.h"
#include "Field.h"
#include "TrajectoryList.h"
#include "static_vector.h"
#include <omp.h>
#include <algorithm>
#include <climits>

using namespace std;

inline void GetPoints(TrajectoryList &Trajectories1, TrajectoryList &Trajectories2, uint TrajectoriesBoard[], static_vector<pos, MAX_CHAIN_POINTS> &Moves)
{
	Trajectories1.ExcludeCompositeTrajectories();
	Trajectories2.ExcludeCompositeTrajectories();

	// Проецируем неисключенные траектории на доску.
	Trajectories1.Project(TrajectoriesBoard);
	Trajectories2.Project(TrajectoriesBoard);
	// Исключаем те траектории, у которых имеется более одной точки, принадлежащей только ей.
	while (Trajectories1.ExcludeUnnecessaryTrajectories(TrajectoriesBoard) || Trajectories2.ExcludeUnnecessaryTrajectories(TrajectoriesBoard));
	// Получаем список точек, входящих в оставшиеся неисключенные траектории.
	Moves.clear();
	Trajectories1.GetPoints(Moves);
	Trajectories2.GetPoints(Moves);

#if ALPHABETA_SORT
	// Сортируем точки по убыванию количества траекторий, в которые они входят.
	sort(Moves.begin(), Moves.end(), [&](uint x, uint y){ return TrajectoriesBoard[x] < TrajectoriesBoard[y]; });
#endif

	// Очищаем доску от проекций.
	Trajectories1.UnProject(TrajectoriesBoard);
	Trajectories2.UnProject(TrajectoriesBoard);
	// После получения списка ходов обратно включаем в рассмотрение все траектории (для следующего уровня рекурсии).
	Trajectories1.IncludeAllTrajectories();
	Trajectories2.IncludeAllTrajectories();
}

// Рекурсивная функция минимакса.
// CurField - поле, на котором ведется поиск лучшего хода.
// TrajectoriesBoard - доска, на которую проецируются траектории. Должна быть заполнена нулями. Нужна для оптимизации.
// Depth - глубина просчета.
// Pos - последний выбранный, но не сделанный ход.
// alpha, beta - интервал оценок, вне которого искать нет смысла.
// На выходе оценка позиции для CurPlayer (до хода Pos).
int Negamax(field &CurField, uint TrajectoriesBoard[], uint Depth, uint Pos, TrajectoryList &LastEnemyTrajectories, TrajectoryList &LastCurrentTrajectories, int alpha, int beta)
{
	int BestEstimate = INT_MIN + 1;
	TrajectoryList Trajectories[2];

	// Делаем ход, выбранный на предыдущем уровне рекурсии, после чего этот ход становится вражеским.
	CurField.do_unsafe_step(Pos);

	if (Depth == 0)
	{
		BestEstimate = CurField.get_score(CurField.get_player());
		CurField.undo_step();
		return -BestEstimate;
	}

	if (CurField.get_d_score() < 0) // Если точка поставлена в окружение.
	{
		CurField.undo_step();
		return INT_MIN + 1; // Для CurPlayer это хорошо, то есть оценка Infinity.
	}

	if (Depth > 1)
		Trajectories[next_player(CurField.get_player())].BuildEnemyTrajectories(CurField, LastEnemyTrajectories, Pos, Depth / 2);
	Trajectories[CurField.get_player()].BuildCurrentTrajectories(CurField, LastCurrentTrajectories, Pos, (Depth + 1) / 2, CurField.get_player());
	//Trajectories[CurField.CurPlayer].BuildTrajectories(CurField, (Depth + 1) / 2, CurField.CurPlayer);
	
	static_vector<pos, MAX_CHAIN_POINTS> Moves;
	GetPoints(Trajectories[0], Trajectories[1], TrajectoriesBoard, Moves);

	for (auto i = Moves.begin(); i < Moves.end(); i++)
	{
		int CurEstimate = Negamax(CurField, TrajectoriesBoard, Depth - 1, *i, Trajectories[CurField.get_player()], Trajectories[next_player(CurField.get_player())], -beta, -alpha);
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
		BestEstimate = CurField.get_score(CurField.get_player());

	CurField.undo_step();

	return -BestEstimate;
}

int GetEnemyEstimate(field &CurField, TrajectoryList &CurrentTrajectories, TrajectoryList &EnemyTrajectories, uint TrajectoriesBoard[], uint Depth)
{
	TrajectoryList TempTrajectories;
	static_vector<pos, MAX_CHAIN_POINTS> Moves;
	int Result = INT_MIN + 1;

	TempTrajectories.BuildEnemyTrajectories(CurField, CurrentTrajectories, 0, (Depth + 1) / 2 - 1);
	GetPoints(EnemyTrajectories, TempTrajectories, TrajectoriesBoard, Moves);

	int alpha = INT_MIN + 1;
	CurField.set_next_player();
	Depth -= 2;
	omp_lock_t lock;
	omp_init_lock(&lock);
	#pragma omp parallel
	{
		int MaxEstimate = INT_MIN + 1, CurEstimate;
		field LocalField(CurField);
		uint TrajectoriesBoard[PointsLength22] = {0};

		#pragma omp for schedule(dynamic, 1)
		for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(Moves.size()); i++)
		{
			CurEstimate = Negamax(LocalField, TrajectoriesBoard, Depth, Moves[i], EnemyTrajectories, CurrentTrajectories, INT_MIN + 1, -alpha);
			if (CurEstimate > alpha) // Обновляем нижнюю границу.
				alpha = CurEstimate;
			if (CurEstimate > MaxEstimate)
				MaxEstimate = CurEstimate;
		}

		omp_set_lock(&lock);
		if (MaxEstimate > Result)
			Result = MaxEstimate;
		omp_unset_lock(&lock);
	}
	omp_destroy_lock(&lock);
	CurField.set_next_player();

	EnemyTrajectories.IncludeAllTrajectories();

	return -Result;
}

// CurField - поле, на котором производится оценка.
// Depth - глубина оценки.
// Moves - на входе возможные ходы, на выходе лучшие из них.
int MinMaxEstimate(field &CurField, uint Depth, static_vector<pos, MAX_CHAIN_POINTS> &Moves)
{
	// Минимально и максимально возможная оценка.
	int BestEstimate = INT_MIN + 1;
	// Главные траектории - свои и вражеские.
	TrajectoryList Trajectories[2];
	// Доска, на которую идет проецирование траекторий. Необходима для оптимизации работы с памятью.
	uint TrajectoriesBoard[PointsLength22] = {0};
	static_vector<pos, MAX_CHAIN_POINTS> BestMoves, PossibleMoves, FirstMoves;
	// Доска с оценками ходов.
	int ScoreBoard[PointsLength22];

	// Делаем что-то только когда глубина просчета положительная и колическтво возможных ходов на входе не равно 0.
	if (Depth <= 0 || Moves.size() == 0)
		return 0;
	// Строим свои траектоии.
	Trajectories[CurField.get_player()].BuildTrajectories(CurField, (Depth + 1) / 2, CurField.get_player());
	// Строим траектории противника.
	if (Depth > 1)
		Trajectories[next_player(CurField.get_player())].BuildTrajectories(CurField, Depth / 2, next_player(CurField.get_player()));
	// Получаем ходы из траекторий (которые имеет смысл рассматривать), и находим пересечение со входными возможными точками.
	GetPoints(Trajectories[0], Trajectories[1], TrajectoriesBoard, PossibleMoves);
	for (auto i = PossibleMoves.begin(); i < PossibleMoves.end(); i++)
		if (find(Moves.begin(), Moves.end(), *i) != Moves.end())
			FirstMoves.push_back(*i);
	// Если нет возможных ходов, входящих в траектории - выходим.
	if (FirstMoves.size() == 0)
		return 0;
	// Для почти всех возможных точек, не входящих в траектории оценка будет такая же, как если бы игрок CurPlayer пропустил ход. Записываем оценку для всех ходов, так как потом для ходов, которые входят в траектории она перезапишется.
	int EnemyEstimate = GetEnemyEstimate(CurField, Trajectories[CurField.get_player()], Trajectories[next_player(CurField.get_player())], TrajectoriesBoard, Depth);
	for (auto i = Moves.begin(); i < Moves.end(); i++)
		ScoreBoard[*i] = EnemyEstimate;

	int alpha = INT_MIN + 2;
	#pragma omp parallel
	{
		field LocalField(CurField);
		uint TrajectoriesBoard[PointsLength22] = {0};

		#pragma omp for schedule(dynamic, 1)
		for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(FirstMoves.size()); i++)
		{
			int CurEstimate = Negamax(LocalField, TrajectoriesBoard, Depth - 1, FirstMoves[i], Trajectories[LocalField.get_player()], Trajectories[next_player(LocalField.get_player())], INT_MIN + 1, -alpha + 1);
			if (CurEstimate > alpha) // Обновляем нижнюю границу.
				alpha = CurEstimate;
			ScoreBoard[FirstMoves[i]] = CurEstimate;
		}
	}

	for (auto i = Moves.begin(); i < Moves.end(); i++)
		if (ScoreBoard[*i] > BestEstimate)
		{
			BestEstimate = ScoreBoard[*i];
			BestMoves.clear();
			BestMoves.push_back(*i);
		}
		else if (ScoreBoard[*i] == BestEstimate)
		{
			BestMoves.push_back(*i);
		}
	Moves.assign(BestMoves.begin(), BestMoves.end());
	return BestEstimate;
}