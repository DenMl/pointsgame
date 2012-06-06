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
		private int[] Dots_;

		#endregion

		#region Constructors

		public MoveGenerator(Field field)
		{
			Field_ = field;
		}

		#endregion

		#region Public Method

		public void GenerateMoves(Dot player)
		{
			Moves = new List<int>(Field_.DotsSequenceCount * 2);
			Dots_ = new int[Field_.RealDotsCount];

			foreach (var dotState in Field_.DotsSequanceStates)
				AddRemoveEmptyPositions(dotState.Move.Position, true);
		}

		public void UpdateMoves()
		{
			if (Field_.LastMoveState == enmMoveState.Add)
			{
				AddRemoveEmptyPositions(Field_.LastPosition, true);
				Moves.Remove(Field_.LastPosition);
			}
			else if (Field_.LastMoveState == enmMoveState.Remove)
			{
				AddRemoveEmptyPositions(Field_.LastPosition, false);
				Moves.Add(Field_.LastPosition);
			}
		}

		#endregion

		#region Helpers

		private void AddRemoveEmptyPositions(int pos, bool add)
		{
			var position = pos - Field.RealWidth * 2 - 2;
			for (int i = 0; i < 5; i++)
			{
				for (int j = position; j < position + 5; j++)
					try
					{
						if (add)
						{
							if (Field_[j].IsPuttingAllowed())
							{
								if (!Moves.Contains(j))
									Moves.Add(j);
								Dots_[j] += 1;
							}
						}
						else
						{
							if (Dots_[j] > 0)
							{
								Dots_[j] -= 1;
								if (Dots_[j] == 0)
									Moves.Remove(j);
							}
						}
					}
					catch
					{
					}

				position += Field.RealWidth;
			}
		}

		#endregion

		#region Properties

		public List<int> Moves
		{
			get;
			private set;
		}

		private List<int> NonEmptyDots
		{
			get
			{
				var result = new List<int>();
				for (int i = 0; i < Dots_.Length; i++)
					if (Dots_[i] != 0)
						result.Add(i);
				return result;
			}
		}

		#endregion
	}
}
