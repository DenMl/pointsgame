#include "config.h"
#include "basic_types.h"
#include "player.h"
#include "field.h"
#include "static_vector.h"
#include <list>
#include <climits>

using namespace std;

const short cg_summa[] = {-5, -1, 0, 0, 1, 2, 5, 20, 30};

// Учет позиционных эвристик.
short position_estimate(field &cur_field, uint cur_pos, short cur_player)
{
	short g1, g2;
	short c1, c2;
	short result;

	g1 = cur_field.number_near_groups(cur_pos, cur_player);
	g2 = cur_field.number_near_groups(cur_pos, next_player(cur_player));
	c1 = cg_summa[cur_field.number_near_points(cur_pos, cur_player)];
	c2 = cg_summa[cur_field.number_near_points(cur_pos, next_player(cur_player))];
	result = (g1 * 3 + g2 * 2) * (5 - abs(g1 - g2)) - c1 - c2;
	if (cur_field.points_seq.size() > 0 && cur_field.is_near(cur_field.points_seq.back(), cur_pos))
		result += 5;
	// Эмпирическая формула оценки важности точки при просчете ходов.
	return result;
}

pos position_estimate(field &cur_field, list<pos> &moves)
{
	short best_estimate = SHRT_MIN;
	pos result = -1;
	for (auto i = moves.begin(); i != moves.end(); i++)
	{
		short cur_estimate = position_estimate(cur_field, *i, cur_field.get_player());
		if (cur_estimate > best_estimate)
		{
			best_estimate = cur_estimate;
			result = *i;
		}
	}
	return result;
}