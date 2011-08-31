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

#if DEBUG
#include <assert.h>
#endif

using namespace std;

inline short PlayRandomGame(field &CurrentField, static_vector<pos, MAX_CHAIN_POINTS> &PossibleMoves)
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
		if (CurrentField.putting_allow(*i))
		{
			CurrentField.do_unsafe_step(*i);
			Putted++;
		}

		if (CurrentField.get_score(player_red) > 0)
			result = player_red;
		else if (CurrentField.get_score(player_black) > 0)
			result = player_black;
		else
			result = -1;

		for (uint i = 0; i < Putted; i++)
			CurrentField.undo_step();

		return result;
}

inline void CreateChildren(field &CurrentField, static_vector<pos, MAX_CHAIN_POINTS> &PossibleMoves, Node &n)
{
	Node **CurrentChild = &n.Child;

	for (auto i = PossibleMoves.begin(); i < PossibleMoves.end(); i++)
		if (CurrentField.putting_allow(*i))
		{
			*CurrentChild = new Node();
			(*CurrentChild)->Move = *i;
			CurrentChild = &(*CurrentChild)->Sibling;
		}
}

inline Node* UCTSelect(Node &n)
{
	double bestuct = 0, winrate, uct, uctvalue;
	Node *result = NULL;
	Node *next = n.Child;
	while (next != NULL)
	{
		if (next->Visits > 0)
		{
			winrate = ((double)next->Wins)/next->Visits;
			uct = UCTK * sqrt(log((double)n.Visits) / (5 * next->Visits));
			uctvalue = winrate + uct;
		}
		else
			uctvalue = 10000 + rand() % 1000;

		if (uctvalue > bestuct)
		{
			bestuct = uctvalue;
			result = next;
		}

		next = next->Sibling;
	}

	return result;
}

short PlaySimulation(field &CurrentField, static_vector<pos, MAX_CHAIN_POINTS> &PossibleMoves, Node &n)
{
	short randomresult;

	if (n.Visits == 0)
	{
		randomresult = PlayRandomGame(CurrentField, PossibleMoves);
	}
	else
	{
		if (n.Child == NULL)
			CreateChildren(CurrentField, PossibleMoves, n);

		Node *next = UCTSelect(n);

		if (next == NULL)
		{
			n.Visits = ULONG_MAX;
			if (CurrentField.get_score(next_player(CurrentField.get_player())) > 0)
				n.Wins = ULONG_MAX;

			if (CurrentField.get_score(player_red) > 0)
				return player_red;
			else if (CurrentField.get_score(player_black) > 0)
				return player_black;
			else
				return -1;
		}

		CurrentField.do_unsafe_step(next->Move);

		randomresult = PlaySimulation(CurrentField, PossibleMoves, *next);

		CurrentField.undo_step();
	}

	n.Visits++;
	if (randomresult == next_player(CurrentField.get_player()))
		n.Wins++;

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
		if (TempField[q.front()] < UCTRadius)
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

void RecursiveFinalUCT(Node *n)
{
	if (n->Child != NULL)
		RecursiveFinalUCT(n->Child);
	if (n->Sibling != NULL)
		RecursiveFinalUCT(n->Sibling);
	delete n;
}

inline void FinalUCT(Node *n)
{
	if (n->Child != NULL)
		RecursiveFinalUCT(n->Child);
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
		Node n;

		field *LocalField = new field(MainField);

		Node **CurChild = &n.Child;
		for (auto i = FirstMoves.begin() + omp_get_thread_num(); i < FirstMoves.end(); i += omp_get_num_threads())
		{
			*CurChild = new Node();
			(*CurChild)->Move = *i;
			CurChild = &(*CurChild)->Sibling;
		}

		for (ulong i = 0; i < MaxSimulations; i++)
			PlaySimulation(*LocalField, PossibleMoves, n);

		omp_set_lock(&lock);
		Node *next = n.Child; 
		while (next != NULL)
		{
			double CurEstimate = (double)next->Wins / next->Visits;
			if (CurEstimate > BestEstimate)
			{
				BestEstimate = CurEstimate;
				Moves.clear();
				Moves.push_back(next->Move);
			}
			else if (CurEstimate == BestEstimate)
			{
				Moves.push_back(next->Move);
			}
			next = next->Sibling;
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
		Node n;

		field *LocalField = new field(MainField);

		Node **CurChild = &n.Child;
		for (auto i = FirstMoves.begin() + omp_get_thread_num(); i < FirstMoves.end(); i += omp_get_num_threads())
		{
			*CurChild = new Node();
			(*CurChild)->Move = *i;
			CurChild = &(*CurChild)->Sibling;
		}

		while (t.get() < Time)
			for (uint i = 0; i < IterationsBeforeCheckTime; i++)
				PlaySimulation(*LocalField, PossibleMoves, n);

		omp_set_lock(&lock);
		Node *next = n.Child; 
		while (next != NULL)
		{
			double CurEstimate = (double)next->Wins / next->Visits;
			if (CurEstimate > BestEstimate)
			{
				BestEstimate = CurEstimate;
				Moves.clear();
				Moves.push_back(next->Move);
			}
			else if (CurEstimate == BestEstimate)
			{
				Moves.push_back(next->Move);
			}
			next = next->Sibling;
		}
		omp_unset_lock(&lock);

		FinalUCT(&n);
		delete LocalField;
	}
	omp_destroy_lock(&lock);

	return BestEstimate;
}