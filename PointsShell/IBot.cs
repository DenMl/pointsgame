using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace PointsShell
{
	interface IBot
	{
		void PutPoint(Pos pos);
		void PutPoint(Pos pos, PlayerColor player);
		void RemoveLastPoint();
		Pos GetBotMovie(string command);
		void SetCurrentPlayer(PlayerColor player);
		PlayerColor GetCurrentPlayer();
		void SetNextPlayer();
	}
}
