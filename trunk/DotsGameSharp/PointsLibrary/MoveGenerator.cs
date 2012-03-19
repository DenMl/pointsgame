using System;
using System.Collections.Generic;
using System.Linq;

namespace PointsLibrary
{
	internal class MoveGenerator
	{
		#region Fields
		
		private Field Field_;

		#endregion

		#region Constructors

		public MoveGenerator(Field field)
		{
			Field_ = field;
		}

		#endregion

		#region Public Method

		public int[] GenerateMovesForPlayer(Dot player)
		{
			var result = new HashSet<int>();

			foreach (var dotState in Field_.DotsSequanceStates)
			{
				//if (Field.Dots[dotState.Move.Position].IsPlayer(player))
				GetAllEmptyPositions(dotState.Move.Position, ref result);
			}

			return result.ToArray();
		}

		#endregion

		#region Helpers

		public void GetAllEmptyPositions(int centerPosition, ref HashSet<int> positions)
		{
			var position = centerPosition - Field.RealWidth - 2;
			for (int i = 0; i < 5; i++)
			{
				for (int j = position; j < position + 4; j++)
				{
					if (Field_[j].IsPuttingAllowed())
						positions.Add(j);
				}

				position += Field.RealWidth;
			}
		}

		#endregion
	}
}
