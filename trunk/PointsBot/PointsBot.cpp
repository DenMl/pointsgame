#include "BasicTypes.h"
#include "PointsBot.h"
#include "Field.h"
#include "Config.h"
#include "BotEngine.h"
#include "Random.h"

Field* InitField(size_t FieldWidth, size_t FieldHeight, size_t SurCond, size_t Pattern)
{
	return new Field((ushort)FieldWidth, (ushort)FieldHeight, (SurroundCondition)SurCond, (BeginPattern)Pattern);
}

void FinalField(Field *MainField)
{
	if (MainField != NULL)
		delete MainField;
}

void PutPoint(Field *MainField, size_t X, size_t Y)
{
	MainField->DoStep(MainField->ConvertToPos((ushort)X, (ushort)Y));
}

void PutPlayersPoint(Field *MainField, size_t X, size_t Y, size_t Player)
{
	MainField->DoStep(MainField->ConvertToPos((ushort)X, (ushort)Y), (short)Player);
}

void RemoveLastPoint(Field *MainField)
{
	MainField->UndoStep();
}

void SetCurrentPlayer(Field *MainField, size_t Player)
{
	MainField->SetCurrentPlayer((short)Player);
}

size_t GetCurrentPlayer(Field *MainField)
{
	return MainField->CurPlayer;
}

void SetNextPlayer(Field *MainField)
{
	MainField->SetNextPlayer();
}

void GetBotMove(Field *MainField, size_t MinMaxDepth, size_t UCTIterations, ushort &X, ushort &Y)
{
	Randomize();
	MainField->ConvertToXY(SearchBestMove(*MainField, (uint)MinMaxDepth, (ulong)UCTIterations), X, Y);
}