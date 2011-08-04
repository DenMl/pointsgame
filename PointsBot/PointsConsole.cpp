#include "Config.h"
#include "Random.h"
#include "Field.h"
#include "BasicTypes.h"
#include "BasicConstants.h"
#include "BotEngine.h"
#include <iostream>
#include <string>
#include <map>

using namespace std;

Field *MainField;

const p_int min_MinMaxDepth = 0;
const p_int max_MinMaxDepth = 8;
const p_int min_UCTIterations = 0;
const p_int max_UCTIterations = 100000;
const p_int min_P = 0;
const p_int max_P = 100;

inline p_int GetMinMaxDepth(p_int P)
{
	return (P - min_P) * (max_MinMaxDepth - min_MinMaxDepth) / (max_P - min_P) + min_MinMaxDepth;
}

inline p_int GetUCTIterations(p_int P)
{
	return (P - min_P) * (max_UCTIterations - min_UCTIterations) / (max_P - min_P) + min_UCTIterations;
}

void boardsize(int id)
{
	p_int X, Y;
	cin >> X >> Y;
	if (MainField != NULL)
		MainField->Initialize(X, Y, Field::Standart, Field::CleanPattern);
	else
		MainField = new Field(X, Y, Field::Standart, Field::CleanPattern);
	cout << "=" << " " << id << " " << "boardsize" << endl;
}

void genmove(int id)
{
	p_int X, Y, color, pos;
	cin >> color;
	if (MainField == NULL)
	{
		cout << "?" << " " << id << " " << "genmove" << endl;
	}
	else
	{
		MainField->SetCurrentPlayer(color);
		pos = SearchBestMove(*MainField, 8, 100000);
		MainField->DoStep(pos);
		MainField->ConvertToXY(pos, X, Y);
		cout << "=" << " " << id << " " << "genmove" << " " << X << " " << Y << " " << color << endl;
	}
}

void list_commands(int id)
{
	cout << "=" << " " << id << " " << "list_commands" << " " << "boardsize genmove list_commands name play quit reg_genmove reg_genmove_with_complexity reg_genmove_with_time undo version" << endl;
}

void name(int id)
{
	cout << "=" << " " << id << " " << "name" << " " << "kkai" << endl;
}

void play(int id)
{
	p_int X, Y, color;
	cin >> X >> Y >> color;
	if (MainField == NULL || !MainField->DoStep(MainField->ConvertToPos(X, Y), color))
		cout << "?" << " " << id << " " << "play" << endl;
	else
		cout << "=" << " " << id << " " << "play" << " " << X << " " << Y << " " << color << endl;
}

void quit(int id)
{
	cout << "=" << " " << id << " " << "quit" << endl;
	exit(0);
}

void reg_genmove(int id)
{
	p_int X, Y, color;
	cin >> color;
	if (MainField == NULL)
	{
		cout << "?" << " " << id << " " << "reg_genmove" << endl;
	}
	else
	{
		MainField->SetCurrentPlayer(color);
		MainField->ConvertToXY(SearchBestMove(*MainField, 8, 100000), X, Y);
		cout << "=" << " " << id << " " << "reg_genmove" << " " << X << " " << Y << " " << color << endl;
	}
}

void reg_genmove_with_complexity(int id)
{
	p_int X, Y, color, P;
	cin >> color >> P;
	if (MainField == NULL)
	{
		cout << "?" << " " << id << " " << "reg_genmove_with_complexity" << endl;
	}
	else
	{
		MainField->SetCurrentPlayer(color);
		MainField->ConvertToXY(SearchBestMove(*MainField, GetMinMaxDepth(P), GetUCTIterations(P)), X, Y);
		cout << "=" << " " << id << " " << "reg_genmove_with_complexity" << " " << X << " " << Y << " " << color << endl;
	}
}

void reg_genmove_with_time(int id)
{
	p_int X, Y, color, time;
	cin >> color >> time;
	if (MainField == NULL)
	{
		cout << "?" << " " << id << " " << "reg_genmove_with_time" << endl;
	}
	else
	{
		MainField->SetCurrentPlayer(color);
		MainField->ConvertToXY(SearchBestMoveWithTime(*MainField, time), X, Y);
		cout << "=" << " " << id << " " << "reg_genmove_with_time" << " " << X << " " << Y << " " << color << endl;
	}
}

void undo(int id)
{
	if (MainField == NULL || MainField->PointsSeq.Count == 0)
	{
		cout << "?" << " " << id << " " << "undo" << endl;
	}
	else
	{
		MainField->UndoStep();
		cout << "=" << " " << id << " " << "undo" << endl;
	}
}

void version(int id)
{
	cout << "=" << " " << id << " " << "version" << " " << "1.4.0.0" << endl;
}

inline void FillCodes(map<string, void(*)(int)> &codes)
{
	codes["boardsize"] = boardsize;
	codes["genmove"] = genmove;
	codes["list_commands"] = list_commands;
	codes["name"] = name;
	codes["play"] = play;
	codes["quit"] = quit;
	codes["reg_genmove"] = reg_genmove;
	codes["reg_genmove_with_complexity"] = reg_genmove_with_complexity;
	codes["reg_genmove_with_time"] = reg_genmove_with_time;
	codes["undo"] = undo;
	codes["version"] = version;
}

p_int main(p_int argc, char* argv[])
{
	string s;
	int id;
	map<string, void(*)(int)> codes;

	MainField = NULL;
	FillCodes(codes);
	Randomize();

	//cout << argv[0] << std::endl;
	
	while (true)
	{
		cin >> id >> s;
		map<string, void(*)(int)>::iterator i = codes.find(s);
		if (i != codes.end())
			i->second(id);
		else
			cout << "?" << " " << id << " " << s << endl;
	}
}