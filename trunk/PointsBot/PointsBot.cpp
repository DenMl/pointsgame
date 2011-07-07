#include "BasicTypes.h"
#include "PointsBot.h"
#include "Field.h"
#include "Config.h"
#include "BotEngine.h"
#include <omp.h>
#include "Random.h"

Field* InitField(_int FieldWidth, _int FieldHeight, _int SurCond, _int BeginPattern)
{
	return new Field(FieldWidth, FieldHeight, SurCond, BeginPattern);
}

void FinalField(Field *MainField)
{
	if (MainField != NULL)
		delete MainField;
}

void PutPoint(Field *MainField, _int X, _int Y)
{
	MainField->DoStep(MainField->ConvertToPos(X, Y));
}

void PutPlayersPoint(Field *MainField, _int X, _int Y, _int Player)
{
	MainField->DoStep(MainField->ConvertToPos(X, Y), Player);
}

void RemoveLastPoint(Field *MainField)
{
	MainField->UndoStep();
}

void SetCurrentPlayer(Field *MainField, _int Player)
{
	MainField->SetCurrentPlayer(Player);
}

_int GetCurrentPlayer(Field *MainField)
{
	return MainField->CurPlayer;
}

void SetNextPlayer(Field *MainField)
{
	MainField->SetNextPlayer();
}

void GetBotMove(Field *MainField, _int MinMaxDepth, _int UCTIterations, _int &X, _int &Y)
{
	Randomize();
	MainField->ConvertToXY(SearchBestMove(*MainField, MinMaxDepth, UCTIterations), X, Y);
}