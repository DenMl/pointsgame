#include "basic_types.h"
#include "PointsBot.h"
#include "Field.h"
#include "Config.h"
#include "bot_engine.h"
#include "Random.h"

field* InitField(size_t FieldWidth, size_t FieldHeight, size_t SurCond, size_t Pattern)
{
	return new field((ushort)FieldWidth, (ushort)FieldHeight, (sur_cond)SurCond, (begin_pattern)Pattern);
}

void FinalField(field *MainField)
{
	if (MainField != NULL)
		delete MainField;
}

void PutPoint(field *MainField, size_t X, size_t Y)
{
	MainField->do_step(MainField->to_pos((ushort)X, (ushort)Y));
}

void PutPlayersPoint(field *MainField, size_t X, size_t Y, size_t Player)
{
	MainField->do_step(MainField->to_pos((ushort)X, (ushort)Y), (short)Player);
}

void RemoveLastPoint(field *MainField)
{
	MainField->undo_step();
}

void SetCurrentPlayer(field *MainField, size_t Player)
{
	MainField->set_player((short)Player);
}

size_t GetCurrentPlayer(field *MainField)
{
	return MainField->get_player();
}

void SetNextPlayer(field *MainField)
{
	MainField->set_next_player();
}

void GetBotMove(field *MainField, size_t MinMaxDepth, size_t UCTIterations, coord &X, coord &Y)
{
	Randomize();
	MainField->to_xy(position_estimate_best_move(*MainField), X, Y);
}