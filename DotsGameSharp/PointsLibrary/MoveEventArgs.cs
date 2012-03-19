using System;
using System.Collections.Generic;
using System.Linq;

namespace PointsLibrary
{
	public class MoveEventArgs : EventArgs
	{
		public MoveAction Action;
		public Dot PlayerColor;
		public int Pos;
		public IEnumerable<int> ChainPoses;
		public IEnumerable<int> SurPoses;

		public MoveEventArgs(MoveAction Action, Dot PlayerColor, int Pos, IEnumerable<int> chainPositions, IEnumerable<int> surroundPositions)
		{
			this.Action = Action;
			this.PlayerColor = PlayerColor;
			this.Pos = Pos;
			this.ChainPoses = chainPositions;
			this.SurPoses = surroundPositions;
		}
	}
}
