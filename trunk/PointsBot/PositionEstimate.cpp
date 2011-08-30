#include "BasicTypes.h"
#include "Config.h"
#include "Player.h"
#include "Field.h"
#include "static_vector.h"
#include <list>
#include <climits>

using namespace std;

const short cgSumma[] = {-5, -1, 0, 0, 1, 2, 5, 20, 30};

// Учет позиционных эвристик.
short PositionEstimate(field &MainField, uint TestedPoint, short Player)
{
	short g1, g2;
	short c1, c2;
	short Result;

	g1 = MainField.number_near_groups(TestedPoint, Player);
	g2 = MainField.number_near_groups(TestedPoint, next_player(Player));
	c1 = cgSumma[MainField.number_near_points(TestedPoint, Player)];
	c2 = cgSumma[MainField.number_near_points(TestedPoint, next_player(Player))];
	Result = (g1 * 3 + g2 * 2) * (5 - abs(g1 - g2)) - c1 - c2;
	if (MainField.points_seq.size() > 0 && MainField.is_near(MainField.points_seq.back(), TestedPoint))
		Result += 5;
	// Эмпирическая формула оценки важности точки при просчете ходов.
	return Result;
}

void PositionEstimate(field &MainField, static_vector<pos, MAX_CHAIN_POINTS> &Moves)
{
	short BestScore = SHRT_MIN;
	static_vector<pos, MAX_CHAIN_POINTS> BestMoves;
	for (auto i = Moves.begin(); i < Moves.end(); i++)
	{
		short TempScore = PositionEstimate(MainField, *i, MainField.get_player());
		if (TempScore > BestScore)
		{
			BestScore = TempScore;
			BestMoves.clear();
			BestMoves.push_back(*i);
		}
		else if (TempScore == BestScore)
		{
			BestMoves.push_back(*i);
		}
	}
	Moves.assign(BestMoves.begin(), BestMoves.end());
}