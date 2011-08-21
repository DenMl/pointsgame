#include "Config.h"
#include "BasicTypes.h"
#include "BotEngine.h"
#include "Field.h"
#include "MinMaxEstimate.h"
#include "UCTEstimate.h"
#include "PositionEstimate.h"
#include "Random.h"
#include "static_vector.h"

using namespace std;

void BuildAllMoves(Field &MainField, static_vector<pos, MAX_CHAIN_POINTS> &Moves)
{
	Moves.clear();
	for (pos i = MainField.MinPos; i <= MainField.MaxPos; i++)
		if (MainField.PuttingAllow(i))
			Moves.push_back(i);
}

pos SearchBestMove(Field &MainField, uint Depth, ulong UCTIterations)
{
	static_vector<pos, MAX_CHAIN_POINTS> Moves;
	BuildAllMoves(MainField, Moves);

	// Если на доске не стоит ни одной точки - возвращаем случайный ход.
	if (MainField.PointsSeq.size() == 0)
		return Moves[rand() % Moves.size()];

	if (Moves.size() == 1) // Если возможный ход один - возвращаем его.
		return Moves[0];
	else if (Moves.size() == 0) // Если нет возможных ходов - возвращаем ошибку -1.
		return -1;

	MinMaxEstimate(MainField, Depth, Moves);

	if (Moves.size() == 1) // Если возможный ход один - возвращаем его.
		return Moves[0];
	else if (Moves.size() == 0) // Если нет возможных ходов - возвращаем ошибку -1.
		return -1;

	UCTEstimate(MainField, UCTIterations, Moves);

	if (Moves.size() == 1) // Если возможный ход один - возвращаем его.
		return Moves[0];
	else if (Moves.size() == 0) // Если нет возможных ходов - возвращаем ошибку -1.
		return -1;

	PositionEstimate(MainField, Moves);

	if (Moves.size() == 1) // Если возможный ход один - возвращаем его.
		return Moves[0];
	else if (Moves.size() == 0) // Если нет возможных ходов - возвращаем ошибку -1.
		return -1;
	else // Если в конце несколько возможных лучших ходов - возвращаем случайный.
		return Moves[rand() % Moves.size()];
}

pos SearchBestMoveWithTime(Field &MainField, ulong Time)
{
	static_vector<pos, MAX_CHAIN_POINTS> Moves;
	BuildAllMoves(MainField, Moves);

	// Если на доске не стоит ни одной точки - возвращаем случайный ход.
	if (MainField.PointsSeq.size() == 0)
		return Moves[rand() % Moves.size()];

	if (Moves.size() == 1) // Если возможный ход один - возвращаем его.
		return Moves[0];
	else if (Moves.size() == 0) // Если нет возможных ходов - возвращаем ошибку -1.
		return -1;

	UCTEstimateWithTime(MainField, Time, Moves);

	if (Moves.size() == 1) // Если возможный ход один - возвращаем его.
		return Moves[0];
	else if (Moves.size() == 0) // Если нет возможных ходов - возвращаем ошибку -1.
		return -1;

	PositionEstimate(MainField, Moves);

	if (Moves.size() == 1) // Если возможный ход один - возвращаем его.
		return Moves[0];
	else if (Moves.size() == 0) // Если нет возможных ходов - возвращаем ошибку -1.
		return -1;
	else // Если в конце несколько возможных лучших ходов - возвращаем случайный.
		return Moves[rand() % Moves.size()];
}