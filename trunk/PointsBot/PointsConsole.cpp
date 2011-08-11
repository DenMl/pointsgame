#include "Config.h"
#include "Random.h"
#include "Field.h"
#include "BasicTypes.h"
#include "BasicConstants.h"
#include "BotEngine.h"
#include <iostream>
#include <string>
#include <map>

Field *MainField;

const uint min_MinMaxDepth = 0;
const uint max_MinMaxDepth = 8;
const ulong min_UCTIterations = 0;
const ulong max_UCTIterations = 100000;
const ulong min_P = 0;
const ulong max_P = 100;

inline uint GetMinMaxDepth(ulong P)
{
	return (P - min_P) * (max_MinMaxDepth - min_MinMaxDepth) / (max_P - min_P) + min_MinMaxDepth;
}

inline ulong GetUCTIterations(ulong P)
{
	return (P - min_P) * (max_UCTIterations - min_UCTIterations) / (max_P - min_P) + min_UCTIterations;
}

void boardsize(int id)
{
	ushort X, Y;
	cin >> X >> Y;
	if (MainField != NULL)
		delete MainField;
	MainField = new Field(X, Y, Standart, CleanPattern);
	cout << "=" << " " << id << " " << "boardsize" << endl;
}

void genmove(int id)
{
	ushort X, Y;
	short color;
	uint pos;
	cin >> color;
	if (MainField == NULL)
	{
		cout << "?" << " " << id << " " << "genmove" << endl;
	}
	else
	{
		MainField->SetCurrentPlayer(color);
		pos = SearchBestMove(*MainField, 8, 100000);
		if (pos == -1)
		{
			cout << "?" << " " << id << " " << "genmove" << endl;
			return;
		}
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
	ushort X, Y;
	short color;
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
	ushort X, Y;
	short color;
	uint pos;
	cin >> color;
	if (MainField == NULL)
	{
		cout << "?" << " " << id << " " << "reg_genmove" << endl;
	}
	else
	{
		MainField->SetCurrentPlayer(color);
		pos = SearchBestMove(*MainField, 8, 100000);
		if (pos == -1)
		{
			cout << "?" << " " << id << " " << "reg_genmove" << endl;
			return;
		}
		MainField->ConvertToXY(pos, X, Y);
		cout << "=" << " " << id << " " << "reg_genmove" << " " << X << " " << Y << " " << color << endl;
	}
}

void reg_genmove_with_complexity(int id)
{
	ushort X, Y;
	short color;
	uint pos;
	ulong P;
	cin >> color >> P;
	if (MainField == NULL)
	{
		cout << "?" << " " << id << " " << "reg_genmove_with_complexity" << endl;
	}
	else
	{
		MainField->SetCurrentPlayer(color);
		pos = SearchBestMove(*MainField, GetMinMaxDepth(P), GetUCTIterations(P));
		if (pos == -1)
		{
			cout << "?" << " " << id << " " << "reg_genmove_with_complexity" << endl;
			return;
		}
		MainField->ConvertToXY(pos, X, Y);
		cout << "=" << " " << id << " " << "reg_genmove_with_complexity" << " " << X << " " << Y << " " << color << endl;
	}
}

void reg_genmove_with_time(int id)
{
	ushort X, Y;
	short color;
	uint pos;
	ulong time;
	cin >> color >> time;
	if (MainField == NULL)
	{
		cout << "?" << " " << id << " " << "reg_genmove_with_time" << endl;
	}
	else
	{
		MainField->SetCurrentPlayer(color);
		pos = SearchBestMoveWithTime(*MainField, time);
		if (pos == -1)
		{
			cout << "?" << " " << id << " " << "reg_genmove_with_time" << endl;
			return;
		}
		MainField->ConvertToXY(pos, X, Y);
		cout << "=" << " " << id << " " << "reg_genmove_with_time" << " " << X << " " << Y << " " << color << endl;
	}
}

void undo(int id)
{
	if (MainField == NULL || MainField->PointsSeq.size() == 0)
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

int main(int argc, char* argv[])
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
		// Очищаем cin до конца строки.
		while (cin.get() != 10) {}
	}
}