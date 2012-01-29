#include "config.h"
#include "basic_types.h"
#include "player.h"
#include "position_estimate.h"
#include <list>
#include <limits>

using namespace std;

// Учет позиционных эвристик.
int position_estimate(field* cur_field, pos cur_pos, player cur_player)
{
	int g1, g2;
	int c1, c2;
	int result;

	g1 = cur_field->number_near_groups(cur_pos, cur_player);
	g2 = cur_field->number_near_groups(cur_pos, next_player(cur_player));
	c1 = cg_summa[cur_field->number_near_points(cur_pos, cur_player)];
	c2 = cg_summa[cur_field->number_near_points(cur_pos, next_player(cur_player))];
	result = (g1 * 3 + g2 * 2) * (5 - abs(g1 - g2)) - c1 - c2;
	if (cur_field->points_seq.size() > 0 && cur_field->is_near(cur_field->points_seq.back(), cur_pos))
		result += 5;
	// Эмпирическая формула оценки важности точки при просчете ходов.
	return result;
}

pos position_estimate(field* cur_field)
{
	int best_estimate = numeric_limits<int>::min();
	pos result = -1;
	for (pos i = cur_field->min_pos(); i <= cur_field->max_pos(); i++)
		if (cur_field->putting_allow(i))
		{
			int cur_estimate = position_estimate(cur_field, i, cur_field->get_player());
			if (cur_estimate > best_estimate)
			{
				best_estimate = cur_estimate;
				result = i;
			}
		}
	return result;
}