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

void BuildAllMoves(field &MainField, static_vector<pos, MAX_CHAIN_POINTS> &Moves)
{
	Moves.clear();
	for (pos i = MainField.min_pos(); i <= MainField.max_pos(); i++)
		if (MainField.putting_allow(i))
			Moves.push_back(i);
}

pos SearchBestMove(field &MainField, uint Depth, ulong UCTIterations)
{
	static_vector<pos, MAX_CHAIN_POINTS> Moves;
	BuildAllMoves(MainField, Moves);

	// Если на доске не стоит ни одной точки - возвращаем случайный ход.
	if (MainField.points_seq.size() == 0)
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

pos SearchBestMoveWithTime(field &MainField, ulong Time)
{
	static_vector<pos, MAX_CHAIN_POINTS> Moves;
	BuildAllMoves(MainField, Moves);

	// Если на доске не стоит ни одной точки - возвращаем случайный ход.
	if (MainField.points_seq.size() == 0)
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