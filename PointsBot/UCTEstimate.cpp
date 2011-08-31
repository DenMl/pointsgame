#include "Config.h"
#include "BasicTypes.h"
#include "UCTEstimate.h"
#include "Player.h"
#include "Field.h"
#include "Random.h"
#include "Time.h"
#include "static_vector.h"
#include <climits>
#include <queue>
#include <algorithm>
#include <omp.h>

using namespace std;

inline short PlayRandomGame(field &cur_field, static_vector<pos, MAX_CHAIN_POINTS> &PossibleMoves)
{
	static_vector<pos, MAX_CHAIN_POINTS> Moves;
	uint Putted = 0;
	short result;

	Moves.fast_resize(PossibleMoves.size());
	Moves[0] = PossibleMoves[0];
	for (uint i = 1; i < PossibleMoves.size(); i++)
	{
		uint j = rand() % (i + 1);
		Moves[i] = Moves[j];
		Moves[j] = PossibleMoves[i];
	}

	for (auto i = Moves.begin(); i < Moves.end(); i++)
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

inline void CreateChildren(field &CurrentField, static_vector<pos, MAX_CHAIN_POINTS> &PossibleMoves, uct_node &n)
{
	uct_node **CurrentChild = &n.child;

	for (auto i = PossibleMoves.begin(); i < PossibleMoves.end(); i++)
		if (CurrentField.putting_allow(*i))
		{
			*CurrentChild = new uct_node();
			(*CurrentChild)->move = *i;
			CurrentChild = &(*CurrentChild)->sibling;
		}
}

inline uct_node* UCTSelect(uct_node &n)
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

short PlaySimulation(field &CurrentField, static_vector<pos, MAX_CHAIN_POINTS> &PossibleMoves, uct_node &n)
{
	short randomresult;

	if (n.visits == 0)
	{
		randomresult = PlayRandomGame(CurrentField, PossibleMoves);
	}
	else
	{
		if (n.child == NULL)
			CreateChildren(CurrentField, PossibleMoves, n);

		uct_node *next = UCTSelect(n);

		if (next == NULL)
		{
			n.visits = ULONG_MAX;
			if (CurrentField.get_score(next_player(CurrentField.get_player())) > 0)
				n.wins = ULONG_MAX;

			if (CurrentField.get_score(player_red) > 0)
				return player_red;
			else if (CurrentField.get_score(player_black) > 0)
				return player_black;
			else
				return -1;
		}

		CurrentField.do_unsafe_step(next->move);

		randomresult = PlaySimulation(CurrentField, PossibleMoves, *next);

		CurrentField.undo_step();
	}

	n.visits++;
	if (randomresult == next_player(CurrentField.get_player()))
		n.wins++;

	return randomresult;
}

inline void GeneratePossibleMoves(field &CurField, static_vector<pos, MAX_CHAIN_POINTS> &PossibleMoves)
{
	ushort TempField[PointsLength22] = {0};
	std::queue<pos> q;

	PossibleMoves.clear();
	for (pos i = CurField.min_pos(); i <= CurField.max_pos(); i++)
		if (CurField.is_putted(i)) //TODO: Класть соседей, а не сами точки.
			q.push(i);

	while (!q.empty())
	{
		if (CurField.putting_allow(q.front())) //TODO: Убрать условие.
			PossibleMoves.push_back(q.front());
		if (TempField[q.front()] < UCT_RADIUS)
		{
			if (CurField.putting_allow(CurField.n(q.front())) && TempField[CurField.n(q.front())] == 0)
			{
				TempField[CurField.n(q.front())] = TempField[q.front()] + 1;
				q.push(CurField.n(q.front()));
			}
			if (CurField.putting_allow(CurField.s(q.front())) && TempField[CurField.s(q.front())] == 0)
			{
				TempField[CurField.s(q.front())] = TempField[q.front()] + 1;
				q.push(CurField.s(q.front()));
			}
			if (CurField.putting_allow(CurField.w(q.front())) && TempField[CurField.w(q.front())] == 0)
			{
				TempField[CurField.w(q.front())] = TempField[q.front()] + 1;
				q.push(CurField.w(q.front()));
			}
			if (CurField.putting_allow(CurField.e(q.front())) && TempField[CurField.e(q.front())] == 0)
			{
				TempField[CurField.e(q.front())] = TempField[q.front()] + 1;
				q.push(CurField.e(q.front()));
			}
		}
		q.pop();
	}
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

double UCTEstimate(field &MainField, ulong MaxSimulations, static_vector<pos, MAX_CHAIN_POINTS> &Moves)
{
	// Список всех возможных ходов для UCT.
	static_vector<pos, MAX_CHAIN_POINTS> PossibleMoves;
	static_vector<pos, MAX_CHAIN_POINTS> FirstMoves;
	double BestEstimate = -1;

	GeneratePossibleMoves(MainField, PossibleMoves);
	for (auto i = Moves.begin(); i < Moves.end(); i++)
		if (find(PossibleMoves.begin(), PossibleMoves.end(), *i) != PossibleMoves.end())
			FirstMoves.push_back(*i);

	omp_lock_t lock;
	omp_init_lock(&lock);
	if (omp_get_max_threads() > FirstMoves.size())
		omp_set_num_threads(FirstMoves.size());
#pragma omp parallel
	{
		uct_node n;

		field *LocalField = new field(MainField);

		uct_node **CurChild = &n.child;
		for (auto i = FirstMoves.begin() + omp_get_thread_num(); i < FirstMoves.end(); i += omp_get_num_threads())
		{
			*CurChild = new uct_node();
			(*CurChild)->move = *i;
			CurChild = &(*CurChild)->sibling;
		}

		for (ulong i = 0; i < MaxSimulations; i++)
			PlaySimulation(*LocalField, PossibleMoves, n);

		omp_set_lock(&lock);
		uct_node *next = n.child; 
		while (next != NULL)
		{
			double CurEstimate = (double)next->wins / next->visits;
			if (CurEstimate > BestEstimate)
			{
				BestEstimate = CurEstimate;
				Moves.clear();
				Moves.push_back(next->move);
			}
			else if (CurEstimate == BestEstimate)
			{
				Moves.push_back(next->move);
			}
			next = next->sibling;
		}
		omp_unset_lock(&lock);

		FinalUCT(&n);
		delete LocalField;
	}
	omp_destroy_lock(&lock);

	return BestEstimate;
}

double UCTEstimateWithTime(field &MainField, ulong Time, static_vector<pos, MAX_CHAIN_POINTS> &Moves)
{
	// Список всех возможных ходов для UCT.
	static_vector<pos, MAX_CHAIN_POINTS> PossibleMoves;
	static_vector<pos, MAX_CHAIN_POINTS> FirstMoves;
	double BestEstimate = -1;
	timer t;

	GeneratePossibleMoves(MainField, PossibleMoves);

	for (auto i = Moves.begin(); i < Moves.end(); i++)
		if (find(PossibleMoves.begin(), PossibleMoves.end(), *i) != PossibleMoves.end())
			FirstMoves.push_back(*i);

	omp_lock_t lock;
	omp_init_lock(&lock);
	if (omp_get_max_threads() > FirstMoves.size())
		omp_set_num_threads(FirstMoves.size());
#pragma omp parallel
	{
		uct_node n;

		field *LocalField = new field(MainField);

		uct_node **CurChild = &n.child;
		for (auto i = FirstMoves.begin() + omp_get_thread_num(); i < FirstMoves.end(); i += omp_get_num_threads())
		{
			*CurChild = new uct_node();
			(*CurChild)->move = *i;
			CurChild = &(*CurChild)->sibling;
		}

		while (t.get() < Time)
			for (uint i = 0; i < UCT_ITERATIONS_BEFORE_CHECK_TIME; i++)
				PlaySimulation(*LocalField, PossibleMoves, n);

		omp_set_lock(&lock);
		uct_node *next = n.child; 
		while (next != NULL)
		{
			double CurEstimate = (double)next->wins / next->visits;
			if (CurEstimate > BestEstimate)
			{
				BestEstimate = CurEstimate;
				Moves.clear();
				Moves.push_back(next->move);
			}
			else if (CurEstimate == BestEstimate)
			{
				Moves.push_back(next->move);
			}
			next = next->sibling;
		}
		omp_unset_lock(&lock);

		FinalUCT(&n);
		delete LocalField;
	}
	omp_destroy_lock(&lock);

	return BestEstimate;
}