using System;
using System.Collections.Generic;
using System.Linq;
using Dots.Library;

namespace Dots.AI
{
	public class MoveGenerator
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
					if (Field_.IsValidPos(j) && Field_[j].IsPuttingAllowed())
						positions.Add(j);
				}

				position += Field.RealWidth;
			}
		}

		#endregion
	}
}
