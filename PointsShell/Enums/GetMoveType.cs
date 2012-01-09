using System;

namespace PointsShell.Enums
{
	[Serializable]
	public enum GetMoveType
	{
		GetMove,
		GetMoveWithComplexity,
		GetMoveWithTime,
		PositionEstimate,
		Minimax,
		MinimaxWithUCT,
		MinimaxWithUCTWithTime,
		UCT,
		UCTWithTime
	}
}
