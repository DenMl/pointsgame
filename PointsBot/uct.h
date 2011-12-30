#pragma once

#include "config.h"
#include "basic_types.h"
#include "field.h"
#include <list>

using namespace std;

struct uct_node
{
	ulong wins;
	ulong visits;
	uint move;
	uct_node* child;
	uct_node* sibling;

	uct_node()
	{
		wins = 0;
		visits = 0;
		move = 0;
		child = NULL;
		sibling = NULL;
	}
};

class uct
{
private:
	field* _field;
	mt* _gen;

	short play_random_game(field &cur_field, vector<pos> &possible_moves);
	void create_children(field &cur_field, vector<pos> &possible_moves, uct_node &n);
	uct_node* uct_select(uct_node &n);
	short play_simulation(field &cur_field, vector<pos> &possible_moves, uct_node &n);
	template<typename _Cont> void generate_possible_moves(field &cur_field, _Cont &possible_moves);
	void recursive_final_uct(uct_node *n);
	void final_uct(uct_node *n);
public:
	uct(field* cur_field, mt* gen);
	pos get(size_t max_simulations, list<pos> &moves);
	pos get_with_time(size_t time, list<pos> &moves);
};