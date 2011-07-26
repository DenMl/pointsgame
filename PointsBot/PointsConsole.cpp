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

inline void FillCodes(map<string, p_int> &codes)
{
	codes["boardsize"] = 1;
	codes["echo"] = 2;
	codes["genmove"] = 3;
	codes["list_commands"] = 4;
	codes["name"] = 5;
	codes["play"] = 6;
	codes["quit"] = 7;
	codes["reg_genmove"] = 8;
	codes["undo"] = 9;
	codes["version"] = 10;
}

inline void boardsize()
{
	p_int X, Y;
	cin >> X >> Y;
	if (MainField != NULL)
		MainField->Initialize(X, Y, Field::Standart, Field::CleanPattern);
	else
		MainField = new Field(X, Y, Field::Standart, Field::CleanPattern);
}

inline void echo()
{
	string s;
	getline(cin, s);
	cout << "message" << s << endl;
}

inline void genmove()
{
	p_int X, Y, color, P, pos;
	cin >> color >> P;
	if (MainField == NULL)
		return;
	MainField->SetCurrentPlayer(color);
	pos = SearchBestMove(*MainField, GetMinMaxDepth(P), GetUCTIterations(P));
	MainField->DoStep(pos);
	MainField->ConvertToXY(pos, X, Y);
	cout << "play " << X << " " << Y << " " << color << endl;
}

inline void list_commands()
{
	cout << "list_commands boardsize echo genmove list_commands name play quit reg_genmove undo version" << endl;
}

inline void name()
{
	cout << "name kkai" << endl;
}

inline void play()
{
	p_int X, Y, color;
	cin >> X >> Y >> color;
	if (MainField == NULL || !MainField->DoStep(MainField->ConvertToPos(X, Y), color))
		return;
	cout << "play " << X << " " << Y << " " << color << endl;
}

inline void reg_genmove()
{
	p_int X, Y, color, P;
	cin >> color >> P;
	if (MainField == NULL)
		return;
	MainField->SetCurrentPlayer(color);
	MainField->ConvertToXY(SearchBestMove(*MainField, GetMinMaxDepth(P), GetUCTIterations(P)), X, Y);
	cout << "reg_genmove " << X << " " << Y << " " << color << endl;
}

inline void undo()
{
	if (MainField == NULL || MainField->PointsSeq.Count == 0)
		return;
	MainField->UndoStep();
}

inline void version()
{
	cout << "version 1.4.0.0" << endl;
}

p_int main(p_int argc, char* argv[])
{
	string s;
	map<string, p_int> codes;

	MainField = NULL;
	FillCodes(codes);
	Randomize();
	list_commands();

	//cout << argv[0] << std::endl;
	
	while (true)
	{
		cin >> s;
		map<string, p_int>::iterator i = codes.find(s);
		if (i == codes.end())
			continue;
		switch (i->second)
		{
		case 1:
			boardsize();
			break;
		case 2:
			echo();
			break;
		case 3:
			genmove();
			break;
		case 4:
			list_commands();
			break;
		case 5:
			name();
			break;
		case 6:
			play();
			break;
		case 7:
			if (MainField != NULL)
				delete MainField;
			return 0;
			break;
		case 8:
			reg_genmove();
			break;
		case 9:
			undo();
			break;
		case 10:
			version();
			break;
		}
	}
}