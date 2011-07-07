#include "Config.h"
#include "BasicTypes.h"
#include "BasicConstants.h"
#include "Field.h"
#include "LadderEstimate.h"

void LadderExcludeEstimate(Field &MainField, GameStack<_int, MAX_CHAIN_POINTS> &Moves)
{
	for (_int i = MainField.MinPos; i <= MainField.MaxPos; i++)
		if (MainField.PuttingAllow(i))
		{
			MainField.DoUnsafeStep(i);
			if (MainField.DCaptureCount == 0)
				for (_int j = i + 1; j <= MainField.MaxPos; j++)
					if (MainField.PuttingAllow(j))
					{
						MainField.DoUnsafeStep(j);

						MainField.UndoStep();
					}
					MainField.UndoStep();
		}
}

_int LadderIncludeEstimate(Field &MainField, GameStack<_int, MAX_CHAIN_POINTS> &Moves)
{
	Moves.Clear();

	return 0;
}