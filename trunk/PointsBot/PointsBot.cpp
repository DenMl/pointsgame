#include "BasicTypes.h"
#include "PointsBot.h"
#include "Field.h"
#include "Config.h"
#include "BotEngine.h"
#include <omp.h>
#include "Random.h"

Field* InitField(p_int FieldWidth, p_int FieldHeight, p_int SurCond, p_int BeginPattern)
{
	return new Field(FieldWidth, FieldHeight, SurCond, BeginPattern);
}

void FinalField(Field *MainField)
{
	if (MainField != NULL)
		delete MainField;
}

void PutPoint(Field *MainField, p_int X, p_int Y)
{
	MainField->DoStep(MainField->ConvertToPos(X, Y));
}

void PutPlayersPoint(Field *MainField, p_int X, p_int Y, p_int Player)
{
	MainField->DoStep(MainField->ConvertToPos(X, Y), Player);
}

void RemoveLastPoint(Field *MainField)
{
	MainField->UndoStep();
}

void SetCurrentPlayer(Field *MainField, p_int Player)
{
	MainField->SetCurrentPlayer(Player);
}

p_int GetCurrentPlayer(Field *MainField)
{
	return MainField->CurPlayer;
}

void SetNextPlayer(Field *MainField)
{
	MainField->SetNextPlayer();
}

void GetBotMove(Field *MainField, p_int MinMaxDepth, p_int UCTIterations, p_int &X, p_int &Y)
{
	Randomize();
	MainField->ConvertToXY(SearchBestMove(*MainField, MinMaxDepth, UCTIterations), X, Y);
}