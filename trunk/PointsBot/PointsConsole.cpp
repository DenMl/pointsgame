#include "Config.h"
#include "Random.h"
#include "Field.h"
#include "BasicTypes.h"
#include "BasicConstants.h"
#include "BotEngine.h"
#include <iostream>

using namespace std;

const int min_MinMaxDepth = 0;
const int max_MinMaxDepth = 8;
const int min_UCTIterations = 0;
const int max_UCTIterations = 100000;
const int min_P = 0;
const int max_P = 100;

inline int GetMinMaxDepth(int P)
{
	return (P - min_P) * (max_MinMaxDepth - min_MinMaxDepth) / (max_P - min_P) + min_MinMaxDepth;
}

inline int GetUCTIterations(int P)
{
	return (P - min_P) * (max_UCTIterations - min_UCTIterations) / (max_P - min_P) + min_UCTIterations;
}

int main(int argc, char* argv[])
{
	Field *MainField;
	int X, Y, P, color, c, pos;
	char s[256];

	Randomize();

	cout << "PointsAI v1.3.1.1\n";

	cout << "Current path:\n";
	cout << argv[0] << std::endl;

	cin >> P;

	if (P != 3)
	{
		cout << "Invalid protocol. The last version at http://code.google.com/p/pointsgame/" << std::endl << "-1" << std::endl;
		return -1;
	}

	cin >> X >> Y;

	MainField = new Field(X, Y, Field::Standart, Field::CleanPattern);

	while (true)
	{
		cin >> c;

		switch (c)
		{
		case -1:
			return 0;
		case 0:
			cin >> X >> Y >> color;
			if (!MainField->DoStep(MainField->ConvertToPos(X, Y), color))
				continue;
			cout << X << " " << Y << " " << color << std::endl;
			break;
		case 1:
			cin >> color >> P;
			MainField->CurPlayer = color;
			MainField->EnemyPlayer = Field::NextPlayer(color);
			pos = SearchBestMove(*MainField, GetMinMaxDepth(P), GetUCTIterations(P));
			MainField->DoStep(pos);
			MainField->ConvertToXY(pos, X, Y);
			cout << X << " " << Y << " " << color << std::endl;
			break;
		case 2:
			cin.getline(s, 256);
			cout << s << std::endl;
			break;
		}
	}
}