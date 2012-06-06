using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Dots.Library;

namespace Dots.AI
{
	public class OldMoveGenerator
	{
		#region Fields

		private Field Field_;

		#endregion

		#region Constructors

		public OldMoveGenerator(Field field)
		{
			Field_ = field;
		}

		#endregion

		public HashSet<int> GenerateMoves(Dot player, HashSet<int> previousMoves = null)
		{
			HashSet<int> resultMoves;

			if (previousMoves == null)
			{
				resultMoves = new HashSet<int>();
				foreach (var dotState in Field_.DotsSequanceStates)
					AddRemoveEmptyPositions(dotState.Move.Position, resultMoves);
			}
			else
			{
				resultMoves = new HashSet<int>(previousMoves);
				AddRemoveEmptyPositions(Field_.LastPosition, resultMoves);
			}

			return resultMoves;
		}

		private void AddRemoveEmptyPositions(int pos, HashSet<int> moves)
		{
			var position = pos - Field.RealWidth - 1;
			for (int i = 0; i < 3; i++)
			{
				for (int j = position; j < position + 3; j++)
					try
					{
						if (Field_[j].IsPuttingAllowed())
							moves.Add(j);
					}
					catch
					{
					}

				position += Field.RealWidth;
			}
			/*
			var position = pos - Field.RealWidth*2 - 2;
			for (int i = 0; i < 5; i++)
			{
				for (int j = position; j < position + 5; j++)
					try
					{
						if (Field_[j].IsPuttingAllowed())
							moves.Add(j);
					}
					catch
					{
					}

				position += Field.RealWidth;
			}*/
		}
	}
}
