#include "Config.h"
#include "BasicTypes.h"
#include "UCTEstimate.h"
#include "Field.h"
#include "Random.h"
#include "Time.h"
#include <queue>
#include <omp.h>

#if DEBUG
#include <assert.h>
#endif

inline short PlayRandomGame(Field &CurrentField, GameStack<uint, MAX_CHAIN_POINTS> &PossibleMoves)
{
	GameStack<uint, MAX_CHAIN_POINTS> Moves;
	uint Putted = 0;
	short result;

	Moves.Count = PossibleMoves.Count;

	Moves.Stack[0] = PossibleMoves.Stack[0];
	for (uint i = 1; i < PossibleMoves.Count; i++)
	{
		uint j = rand() % (i + 1);
		Moves.Stack[i] = Moves.Stack[j];
		Moves.Stack[j] = PossibleMoves.Stack[i];
	}

	for (uint i = 0; i < Moves.Count; i++)
		if (CurrentField.PuttingAllow(Moves.Stack[i]))
		{
			CurrentField.DoUnsafeStep(Moves.Stack[i]);
			Putted++;
		}

	if (CurrentField.CaptureCount[0] > CurrentField.CaptureCount[1])
		result = 0;
	else if (CurrentField.CaptureCount[0] < CurrentField.CaptureCount[1])
		result = 1;
	else
		result = -1;

	for (uint i = 0; i < Putted; i++)
		CurrentField.UndoStep();

	return result;
}

inline void CreateChildren(Field &CurrentField, GameStack<uint, MAX_CHAIN_POINTS> &PossibleMoves, Node &n)
{
	Node **CurrentChild = &n.Child;

	for (uint i = 0; i < PossibleMoves.Count; i++)
		if (CurrentField.PuttingAllow(PossibleMoves.Stack[i]))
		{
			*CurrentChild = new Node();
			(*CurrentChild)->Move = PossibleMoves.Stack[i];
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
			winrate = (double)(next->Wins)/next->Visits;
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

short PlaySimulation(Field &CurrentField, GameStack<uint, MAX_CHAIN_POINTS> &PossibleMoves, Node &n)
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
			n.Visits = INFINITY;
			if (CurrentField.CaptureCount[CurrentField.EnemyPlayer] > CurrentField.CaptureCount[CurrentField.CurPlayer])
				n.Wins = INFINITY;

			if (CurrentField.CaptureCount[0] > CurrentField.CaptureCount[1])
				return 0;
			else if (CurrentField.CaptureCount[0] < CurrentField.CaptureCount[1])
				return 1;
			else
				return -1;
		}

		CurrentField.DoUnsafeStep(next->Move);

		randomresult = PlaySimulation(CurrentField, PossibleMoves, *next);

		CurrentField.UndoStep();
	}

	n.Visits++;
	if (randomresult == CurrentField.EnemyPlayer)
		n.Wins++;

	return randomresult;
}

inline void GeneratePossibleMoves(Field &CurField, GameStack<uint, MAX_CHAIN_POINTS> &PossibleMoves)
{
	ushort TempField[PointsLength22] = {0};
	std::queue<uint> q;

	PossibleMoves.Clear();
	for (uint i = CurField.MinPos; i <= CurField.MaxPos; i++)
		if (CurField.IsPutted(i)) //TODO: Классть соседей, а не сами точки.
			q.push(i);

	while (!q.empty())
	{
		if (CurField.PuttingAllow(q.front())) //TODO: Убрать условие.
			PossibleMoves.Push(q.front());
		if (TempField[q.front()] < UCTRadius)
		{
			if (CurField.PuttingAllow(q.front() - FieldWidth2 - 1) && TempField[q.front() - FieldWidth2 - 1] == 0)
			{
				TempField[q.front() - FieldWidth2 - 1] = TempField[q.front()] + 1;
				q.push(q.front() - FieldWidth2 - 1);
			}
			if (CurField.PuttingAllow(q.front() - FieldWidth2) && TempField[q.front() - FieldWidth2] == 0)
			{
				TempField[q.front() - FieldWidth2] = TempField[q.front()] + 1;
				q.push(q.front() - FieldWidth2);
			}
			if (CurField.PuttingAllow(q.front() - FieldWidth2 + 1) && TempField[q.front() - FieldWidth2 + 1] == 0)
			{
				TempField[q.front() - FieldWidth2 + 1] = TempField[q.front()] + 1;
				q.push(q.front() - FieldWidth2 + 1);
			}
			if (CurField.PuttingAllow(q.front() - 1) && TempField[q.front() - 1] == 0)
			{
				TempField[q.front() - 1] = TempField[q.front()] + 1;
				q.push(q.front() - 1);
			}
			if (CurField.PuttingAllow(q.front() + 1) && TempField[q.front() + 1] == 0)
			{
				TempField[q.front() + 1] = TempField[q.front()] + 1;
				q.push(q.front() + 1);
			}
			if (CurField.PuttingAllow(q.front() + FieldWidth2 - 1) && TempField[q.front() + FieldWidth2 - 1] == 0)
			{
				TempField[q.front() + FieldWidth2 - 1] = TempField[q.front()] + 1;
				q.push(q.front() + FieldWidth2 - 1);
			}
			if (CurField.PuttingAllow(q.front() + FieldWidth2) && TempField[q.front() + FieldWidth2] == 0)
			{
				TempField[q.front() + FieldWidth2] = TempField[q.front()] + 1;
				q.push(q.front() + FieldWidth2);
			}
			if (CurField.PuttingAllow(q.front() + FieldWidth2 + 1) && TempField[q.front() + FieldWidth2 + 1] == 0)
			{
				TempField[q.front() + FieldWidth2 + 1] = TempField[q.front()] + 1;
				q.push(q.front() + FieldWidth2 + 1);
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

double UCTEstimate(Field &MainField, ulong MaxSimulations, GameStack<uint, MAX_CHAIN_POINTS> &Moves)
{
	// Список всех возможных ходов для UCT.
	GameStack<uint, MAX_CHAIN_POINTS> PossibleMoves;
	double BestEstimate = -1;

	GeneratePossibleMoves(MainField, PossibleMoves);
	Moves.Intersect(PossibleMoves);

	omp_lock_t lock;
	omp_init_lock(&lock);
	if (omp_get_max_threads() > Moves.Count)
		omp_set_num_threads(Moves.Count);
	#pragma omp parallel
	{
		Node n;

		Field *LocalField = new Field(MainField);
#if !ABSOLUTE_UCT
		LocalField->CaptureCount[0] = 0;
		LocalField->CaptureCount[1] = 0;
#endif

		Node **CurChild = &n.Child;
		for (uint i = omp_get_thread_num(); i < Moves.Count; i += omp_get_num_threads())
		{
			*CurChild = new Node();
			(*CurChild)->Move = Moves.Stack[i];
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
				Moves.Clear();
				Moves.Push(next->Move);
			}
			else if (CurEstimate == BestEstimate)
			{
				Moves.Push(next->Move);
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

double UCTEstimateWithTime(Field &MainField, ulong Time, GameStack<uint, MAX_CHAIN_POINTS> &Moves)
{
	// Список всех возможных ходов для UCT.
	GameStack<uint, MAX_CHAIN_POINTS> PossibleMoves;
	double BestEstimate = -1;
	ulong t0 = GetTime();

	GeneratePossibleMoves(MainField, PossibleMoves);
	Moves.Intersect(PossibleMoves);

	omp_lock_t lock;
	omp_init_lock(&lock);
	if (omp_get_max_threads() > Moves.Count)
		omp_set_num_threads(Moves.Count);
#pragma omp parallel
	{
		Node n;

		Field *LocalField = new Field(MainField);
#if !ABSOLUTE_UCT
		LocalField->CaptureCount[0] = 0;
		LocalField->CaptureCount[1] = 0;
#endif

		Node **CurChild = &n.Child;
		for (uint i = omp_get_thread_num(); i < Moves.Count; i += omp_get_num_threads())
		{
			*CurChild = new Node();
			(*CurChild)->Move = Moves.Stack[i];
			CurChild = &(*CurChild)->Sibling;
		}

		while (GetTime() - t0 < Time)
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
				Moves.Clear();
				Moves.Push(next->Move);
			}
			else if (CurEstimate == BestEstimate)
			{
				Moves.Push(next->Move);
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