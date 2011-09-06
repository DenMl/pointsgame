#include "config.h"
#include "basic_types.h"
#include "uct.h"
#include "player.h"
#include "field.h"
#include "Random.h"
#include "time.h"
#include <climits>
#include <queue>
#include <vector>
#include <algorithm>
#include <omp.h>

using namespace std;

inline short play_random_game(field &cur_field, vector<pos> &possible_moves)
{
	vector<pos> moves(possible_moves.size());
	uint Putted = 0;
	short result;

	moves[0] = possible_moves[0];
	for (uint i = 1; i < possible_moves.size(); i++)
	{
		uint j = rand() % (i + 1);
		moves[i] = moves[j];
		moves[j] = possible_moves[i];
	}

	for (auto i = moves.begin(); i < moves.end(); i++)
		if (cur_field.putting_allow(*i))
		{
			cur_field.do_unsafe_step(*i);
			Putted++;
		}

		if (cur_field.get_score(player_red) > 0)
			result = player_red;
		else if (cur_field.get_score(player_black) > 0)
			result = player_black;
		else
			result = -1;

		for (uint i = 0; i < Putted; i++)
			cur_field.undo_step();

		return result;
}

inline void create_children(field &cur_field, vector<pos> &possible_moves, uct_node &n)
{
	uct_node **cur_child = &n.child;

	for (auto i = possible_moves.begin(); i < possible_moves.end(); i++)
		if (cur_field.putting_allow(*i))
		{
			*cur_child = new uct_node();
			(*cur_child)->move = *i;
			cur_child = &(*cur_child)->sibling;
		}
}

inline uct_node* uct_select(uct_node &n)
{
	double bestuct = 0, winrate, uct, uctvalue;
	uct_node *result = NULL;
	uct_node *next = n.child;
	while (next != NULL)
	{
		if (next->visits > 0)
		{
			winrate = ((double)next->wins)/next->visits;
			uct = UCTK * sqrt(log((double)n.visits) / (5 * next->visits));
			uctvalue = winrate + uct;
		}
		else
			uctvalue = 10000 + rand() % 1000;

		if (uctvalue > bestuct)
		{
			bestuct = uctvalue;
			result = next;
		}

		next = next->sibling;
	}

	return result;
}

short play_simulation(field &cur_field, vector<pos> &possible_moves, uct_node &n)
{
	short randomresult;

	if (n.visits == 0)
	{
		randomresult = play_random_game(cur_field, possible_moves);
	}
	else
	{
		if (n.child == NULL)
			create_children(cur_field, possible_moves, n);

		uct_node *next = uct_select(n);

		if (next == NULL)
		{
			n.visits = ULONG_MAX;
			if (cur_field.get_score(next_player(cur_field.get_player())) > 0)
				n.wins = ULONG_MAX;

			if (cur_field.get_score(player_red) > 0)
				return player_red;
			else if (cur_field.get_score(player_black) > 0)
				return player_black;
			else
				return -1;
		}

		cur_field.do_unsafe_step(next->move);

		randomresult = play_simulation(cur_field, possible_moves, *next);

		cur_field.undo_step();
	}

	n.visits++;
	if (randomresult == next_player(cur_field.get_player()))
		n.wins++;

	return randomresult;
}

template<typename _Cont>
inline void generate_possible_moves(field &cur_field, _Cont &possible_moves)
{
	ushort* r_field = new ushort[cur_field.length()];
	fill_n(r_field, cur_field.length(), 0);
	std::queue<pos> q;

	possible_moves.clear();
	for (pos i = cur_field.min_pos(); i <= cur_field.max_pos(); i++)
		if (cur_field.is_putted(i)) //TODO: ������ �������, � �� ���� �����.
			q.push(i);

	while (!q.empty())
	{
		if (cur_field.putting_allow(q.front())) //TODO: ������ �������.
			possible_moves.push_back(q.front());
		if (r_field[q.front()] < UCT_RADIUS)
		{
			if (cur_field.putting_allow(cur_field.n(q.front())) && r_field[cur_field.n(q.front())] == 0)
			{
				r_field[cur_field.n(q.front())] = r_field[q.front()] + 1;
				q.push(cur_field.n(q.front()));
			}
			if (cur_field.putting_allow(cur_field.s(q.front())) && r_field[cur_field.s(q.front())] == 0)
			{
				r_field[cur_field.s(q.front())] = r_field[q.front()] + 1;
				q.push(cur_field.s(q.front()));
			}
			if (cur_field.putting_allow(cur_field.w(q.front())) && r_field[cur_field.w(q.front())] == 0)
			{
				r_field[cur_field.w(q.front())] = r_field[q.front()] + 1;
				q.push(cur_field.w(q.front()));
			}
			if (cur_field.putting_allow(cur_field.e(q.front())) && r_field[cur_field.e(q.front())] == 0)
			{
				r_field[cur_field.e(q.front())] = r_field[q.front()] + 1;
				q.push(cur_field.e(q.front()));
			}
		}
		q.pop();
	}
	delete r_field;
}

void RecursiveFinalUCT(uct_node *n)
{
	if (n->child != NULL)
		RecursiveFinalUCT(n->child);
	if (n->sibling != NULL)
		RecursiveFinalUCT(n->sibling);
	delete n;
}

inline void FinalUCT(uct_node *n)
{
	if (n->child != NULL)
		RecursiveFinalUCT(n->child);
}

pos uct(field &cur_field, size_t max_simulations, list<pos> &moves)
{
	// ������ ���� ��������� ����� ��� UCT.
	vector<pos> possible_moves, first_moves;
	double best_estimate = -1;
	pos result = -1;

	generate_possible_moves(cur_field, possible_moves);
	for (auto i = moves.begin(); i != moves.end(); i++)
		if (find(possible_moves.begin(), possible_moves.end(), *i) != possible_moves.end())
			first_moves.push_back(*i);

	omp_lock_t lock;
	omp_init_lock(&lock);
	if (omp_get_max_threads() > first_moves.size())
		omp_set_num_threads(first_moves.size());
	#pragma omp parallel
	{
		uct_node n;

		field *local_field = new field(cur_field);

		uct_node **cur_child = &n.child;
		for (auto i = first_moves.begin() + omp_get_thread_num(); i < first_moves.end(); i += omp_get_num_threads())
		{
			*cur_child = new uct_node();
			(*cur_child)->move = *i;
			cur_child = &(*cur_child)->sibling;
		}

		for (ulong i = 0; i < max_simulations; i++)
			play_simulation(*local_field, possible_moves, n);

		omp_set_lock(&lock);
		uct_node *next = n.child; 
		while (next != NULL)
		{
			double cur_estimate = (double)next->wins / next->visits;
			if (cur_estimate > best_estimate)
			{
				best_estimate = cur_estimate;
				result = next->move;
			}
			next = next->sibling;
		}
		omp_unset_lock(&lock);

		FinalUCT(&n);
		delete local_field;
	}
	omp_destroy_lock(&lock);

	return result;
}

pos uct_with_time(field &cur_field, size_t time, list<pos> &moves)
{
	// ������ ���� ��������� ����� ��� UCT.
	vector<pos> possible_moves, first_moves;
	double best_estimate = -1;
	pos result = -1;
	timer t;

	generate_possible_moves(cur_field, possible_moves);
	for (auto i = moves.begin(); i != moves.end(); i++)
		if (find(possible_moves.begin(), possible_moves.end(), *i) != possible_moves.end())
			first_moves.push_back(*i);

	omp_lock_t lock;
	omp_init_lock(&lock);
	if (omp_get_max_threads() > first_moves.size())
		omp_set_num_threads(first_moves.size());
	#pragma omp parallel
	{
		uct_node n;

		field *local_field = new field(cur_field);

		uct_node **cur_child = &n.child;
		for (auto i = first_moves.begin() + omp_get_thread_num(); i < first_moves.end(); i += omp_get_num_threads())
		{
			*cur_child = new uct_node();
			(*cur_child)->move = *i;
			cur_child = &(*cur_child)->sibling;
		}

		while (t.get() < time)
			for (uint i = 0; i < UCT_ITERATIONS_BEFORE_CHECK_TIME; i++)
				play_simulation(*local_field, possible_moves, n);

		omp_set_lock(&lock);
		uct_node *next = n.child; 
		while (next != NULL)
		{
			double cur_estimate = (double)next->wins / next->visits;
			if (cur_estimate > best_estimate)
			{
				best_estimate = cur_estimate;
				result = next->move;
			}
			next = next->sibling;
		}
		omp_unset_lock(&lock);

		FinalUCT(&n);
		delete local_field;
	}
	omp_destroy_lock(&lock);

	return result;
}