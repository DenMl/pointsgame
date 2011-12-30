#include "config.h"
#include "basic_types.h"
#include "player.h"
#include "position_estimate.h"
#include <list>
#include <climits>

using namespace std;

position_estimate::position_estimate(field* cur_field)
{
	_field = cur_field;
}

// Учет позиционных эвристик.
short position_estimate::get_estimate(uint cur_pos, short cur_player)
{
	short g1, g2;
	short c1, c2;
	short result;

	g1 = _field->number_near_groups(cur_pos, cur_player);
	g2 = _field->number_near_groups(cur_pos, next_player(cur_player));
	c1 = cg_summa[_field->number_near_points(cur_pos, cur_player)];
	c2 = cg_summa[_field->number_near_points(cur_pos, next_player(cur_player))];
	result = (g1 * 3 + g2 * 2) * (5 - abs(g1 - g2)) - c1 - c2;
	if (_field->points_seq.size() > 0 && _field->is_near(_field->points_seq.back(), cur_pos))
		result += 5;
	// Эмпирическая формула оценки важности точки при просчете ходов.
	return result;
}

pos position_estimate::get(list<pos> &moves)
{
	short best_estimate = SHRT_MIN;
	pos result = -1;
	for (auto i = moves.begin(); i != moves.end(); i++)
	{
		short cur_estimate = get_estimate(*i, _field->get_player());
		if (cur_estimate > best_estimate)
		{
			best_estimate = cur_estimate;
			result = *i;
		}
	}
	return result;
}