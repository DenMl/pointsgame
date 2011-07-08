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

	if (P != 4)
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
		// -1 - выход.
		case -1:
			return 0;

		// 0 - сделать ход.
		// Входные параметры:
		// 1. X - координата x.
		// 2. Y - координата y.
		// 3. color - цвет игрока.
		// Выходные параметры:
		// Повторяет ход, если согласен с ним.
		case 0:
			cin >> X >> Y >> color;
			if (!MainField->DoStep(MainField->ConvertToPos(X, Y), color))
				continue;
			cout << X << " " << Y << " " << color << std::endl;
			break;

		// 1 - получить наилучший ход для текущей позиции.
		// Входные параметры:
		// 1. color - цвет игрока, чей ход нужно получить.
		// 2. P - мощность анализа. Изменяется от 0 до 100.
		// Выходные параметры:
		// Наилучший ход.
		case 1:
			cin >> color >> P;
			MainField->CurPlayer = color;
			MainField->EnemyPlayer = Field::NextPlayer(color);
			pos = SearchBestMove(*MainField, GetMinMaxDepth(P), GetUCTIterations(P));
			MainField->DoStep(pos);
			MainField->ConvertToXY(pos, X, Y);
			cout << X << " " << Y << " " << color << std::endl;
			break;

		// 2 - откатить на один ход назад.
		case 2:
			if (MainField->PointsSeq.Count)
				MainField->UndoStep();
			break;

		// 3 - эхо. Повторяет полученную строку.
		case 3:
			cin.getline(s, 256);
			cout << s << std::endl;
			break;
		}
	}
}