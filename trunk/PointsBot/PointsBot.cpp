#include "BasicTypes.h"
#include "PointsBot.h"
#include "Field.h"
#include "Config.h"
#include "BotEngine.h"
#include "Random.h"

Field* InitField(ushort FieldWidth, ushort FieldHeight, SurroundCondition SurCond, BeginPattern Pattern)
{
	return new Field(FieldWidth, FieldHeight, SurCond, Pattern);
}

void FinalField(Field *MainField)
{
	if (MainField != NULL)
		delete MainField;
}

void PutPoint(Field *MainField, ushort X, ushort Y)
{
	MainField->DoStep(MainField->ConvertToPos(X, Y));
}

void PutPlayersPoint(Field *MainField, ushort X, ushort Y, short Player)
{
	MainField->DoStep(MainField->ConvertToPos(X, Y), Player);
}

void RemoveLastPoint(Field *MainField)
{
	MainField->UndoStep();
}

void SetCurrentPlayer(Field *MainField, short Player)
{
	MainField->SetCurrentPlayer(Player);
}

short GetCurrentPlayer(Field *MainField)
{
	return MainField->CurPlayer;
}

void SetNextPlayer(Field *MainField)
{
	MainField->SetNextPlayer();
}

void GetBotMove(Field *MainField, uint MinMaxDepth, ulong UCTIterations, ushort &X, ushort &Y)
{
	Randomize();
	MainField->ConvertToXY(SearchBestMove(*MainField, MinMaxDepth, UCTIterations), X, Y);
}