#include "Config.h"
#include "UCTEstimate.h"
#include "Field.h"
#include "Random.h"

#if DEBUG
#include <assert.h>
#endif

inline p_int PlayRandomGame(Field &CurrentField, GameStack<p_int, MAX_CHAIN_POINTS> &PossibleMoves)
{
	GameStack<p_int, MAX_CHAIN_POINTS> Moves;
	p_int Putted = 0;
	p_int result;

	Moves.Count = PossibleMoves.Count;

	Moves.Stack[0] = PossibleMoves.Stack[0];
	for (p_int i = 1; i < PossibleMoves.Count; i++)
	{
		p_int j = rand() % (i + 1);
		Moves.Stack[i] = Moves.Stack[j];
		Moves.Stack[j] = PossibleMoves.Stack[i];
	}

	for (p_int i = 0; i < Moves.Count; i++)
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

	for (p_int i = 0; i < Putted; i++)
		CurrentField.UndoStep();

	return result;
}

inline void CreateChildren(Field &CurrentField, GameStack<p_int, MAX_CHAIN_POINTS> &PossibleMoves, Node &n)
{
	Node **CurrentChild = &n.Child;

	for (p_int i = 0; i < PossibleMoves.Count; i++)
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

p_int PlaySimulation(Field &CurrentField, GameStack<p_int, MAX_CHAIN_POINTS> &PossibleMoves, Node &n)
{
	p_int randomresult;

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

inline void SetDefaultRect(Field &CurrentField, Rect &rect)
{
	rect.Left = CurrentField.FieldWidth;
	rect.Right = -1;
	rect.Top = CurrentField.FieldHeight;
	rect.Bottom = -1;
}

inline void SetRect(Field &CurrentField, Rect &rect)
{
	p_int X, Y;
	for (p_int i = 0; i < CurrentField.PointsSeq.Count; i++)
	{
		CurrentField.ConvertToXY(CurrentField.PointsSeq.Stack[i], X, Y);
		if (X < rect.Left)
			rect.Left = X;
		if (X > rect.Right)
			rect.Right = X;
		if (Y < rect.Top)
			rect.Top = Y;
		if (Y > rect.Bottom)
			rect.Bottom = Y;
	}
}

inline void IncreaseRect(Field &CurrentField, Rect &rect)
{
	if (rect.Left > 0)
		rect.Left--;
	if (rect.Right < CurrentField.FieldWidth)
		rect.Right++;
	if (rect.Top > 0)
		rect.Top--;
	if (rect.Bottom < CurrentField.FieldHeight)
		rect.Bottom++;
}

inline void GeneratePossibleMoves(Field &CurrentField, GameStack<p_int, MAX_CHAIN_POINTS> &PossibleMoves, Rect &rect)
{
	PossibleMoves.Clear();
	for (p_int i = rect.Left; i <= rect.Right; i++)
		for (p_int j = rect.Top; j <= rect.Bottom; j++)
		{
			p_int Pos = CurrentField.UnsafeConvertToPos(i, j);
			if (CurrentField.PuttingAllow(Pos))
				PossibleMoves.Push(Pos);
		}
}

inline void InitUCT(Field &CurrentField, GameStack<p_int, MAX_CHAIN_POINTS> &PossibleMoves)
{
	Rect rect;

	SetDefaultRect(CurrentField, rect);
	SetRect(CurrentField, rect);
	IncreaseRect(CurrentField, rect);
	GeneratePossibleMoves(CurrentField, PossibleMoves, rect);
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

void UCTEstimate(Field &MainField, p_int MaxSimulations, GameStack<p_int, MAX_CHAIN_POINTS> &Moves)
{
	p_int Wins[PointsLength22] = {0};
	p_int Visits[PointsLength22] = {0};
	// Список всех возможных ходов для UCT.
	GameStack<p_int, MAX_CHAIN_POINTS> PossibleMoves;

	InitUCT(MainField, PossibleMoves);
	Moves.Intersect(PossibleMoves);

	#pragma omp parallel
	{
		Node n;

		Field *LocalField = new Field(MainField);
#if !ABSOLUTE_UCT
		LocalField->CaptureCount[0] = 0;
		LocalField->CaptureCount[1] = 0;
#endif

		CreateChildren(*LocalField, Moves, n);

		#pragma omp for
		for (p_int i = 0; i < MaxSimulations; i++)
			PlaySimulation(*LocalField, PossibleMoves, n);

		Node *next = n.Child;
		while (next != NULL)
		{
			Wins[next->Move] += next->Wins;
			Visits[next->Move] += next->Visits;

			next = next->Sibling;
		}

		FinalUCT(&n);

		delete LocalField;
	}

	float BestScore = 0;
	GameStack<p_int, MAX_CHAIN_POINTS> BestMoves;
	BestMoves.Clear();
	for (p_int i = 0; i < Moves.Count; i++)
		if (Visits[Moves.Stack[i]] != 0)
		{
			float TempScore = (float)Wins[Moves.Stack[i]] / Visits[Moves.Stack[i]];
			if (TempScore > BestScore)
			{
				BestScore = TempScore;
				BestMoves.Clear();
				BestMoves.Push(Moves.Stack[i]);
			}
			else if (TempScore == BestScore)
			{
				BestMoves.Push(Moves.Stack[i]);
			}
		}
		if (BestMoves.Count != 0)
			Moves.Copy(BestMoves);
}