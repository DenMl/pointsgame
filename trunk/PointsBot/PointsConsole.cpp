#include "Config.h"
#include "Random.h"
#include "Field.h"
#include "BasicTypes.h"
#include "BasicConstants.h"
#include "BotEngine.h"

#include <iostream>
using std::cin;
using std::cout;

int main(int argc, char* argv[])
{
	Field *MainField;
	int X, Y, P, color;

	Randomize();

	cout << "PointsAI v1.3.1.0\n";

	cout << "Current path:\n";
	cout << argv[0] <<std::endl;

	cin >> P;

	if (P != 2)
		return -1;

	cin >> X >> Y;

	MainField = new Field(X, Y, Field::Standart, Field::CleanPattern);

	while (true)
	{
		cin >> X;
		if (X == -1)
			return 0;
		cin >> Y >> color >> P;

		if (!MainField->DoStep(MainField->ConvertToPos(X, Y), color))
			continue;

		cout << X << " " << Y << " " << color << std::endl;

		if (P)
		{
			MainField->CurPlayer = Field::NextPlayer(color);
			MainField->EnemyPlayer = color;
			int pos = SearchBestMove(*MainField, 8, 60000);

			MainField->DoStep(pos);

			MainField->ConvertToXY(pos, X, Y);
			cout << X << " " << Y << " " << Field::NextPlayer(color) << std::endl;
		}
	}
}