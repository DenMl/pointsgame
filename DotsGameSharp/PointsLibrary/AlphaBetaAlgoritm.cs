using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Dots.Library;

namespace Dots.AI
{
	class AlphaBetaAlgoritm
	{
		#region Fields

		private Field Field_;
		private MoveGenerator MoveGenerator_;

		#endregion

		#region Constructors

		public AlphaBetaAlgoritm(Field field)
		{
			Field_ = field;
			MoveGenerator_ = new MoveGenerator(field);
		}

		#endregion

		#region Public Methods

		public int SearchBestMove()
		{
			return SearchBestMove(4, Field_.CurrentPlayer, -100, -100);
		}

		public int SearchBestMove(int depth, Dot player, float alpha, float beta)
		{
			int bestMove = 0;

			var moves = MoveGenerator_.GenerateMovesForPlayer(player);
			Dot nextPlayer = player.NextPlayer();

			foreach (var move in moves)
			{
				if (alpha < beta)
				{
					Field_.MakeMove(move);
					float tmp = -EvaluatePosition(depth - 1, nextPlayer, -beta, -alpha);
					Field_.UnmakeMove();
					if (tmp > alpha)
					{
						alpha = tmp;
						bestMove = move;
					}
				}
			}

			return bestMove;
		}

		#endregion

		#region Helpers

		private float EvaluatePosition(int depth, Dot player, float alpha, float beta)
		{
			if (depth == 0)
				return player == Dot.RedPlayer ? Field_.RedCaptureCount - Field_.BlueCaptureCount :
					Field_.BlueCaptureCount - Field_.RedCaptureCount;

			var moves = MoveGenerator_.GenerateMovesForPlayer(player);
			Dot nextPlayer = player.NextPlayer();

			foreach (var move in moves)
			{
				if (alpha < beta)
				{
					Field_.MakeMove(move);
					float tmp = -EvaluatePosition(depth - 1, nextPlayer, -beta, -alpha);
					Field_.UnmakeMove();
					if (tmp > alpha)
						alpha = tmp;
				}
			}

			return alpha;
		}

		#endregion
	}
}
