using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Dots.Library;

namespace Dots.AI
{
	public class AlphaBetaAlgoritm
	{
		#region Constructors

		public AlphaBetaAlgoritm(Field field, MoveGenerator moveGenerator = null, Estimator estimator = null)
		{
			Field = field;
			MoveGenerator = moveGenerator ?? new MoveGenerator(field);
			Estimator = estimator ?? new Estimator(field);
		}

		#endregion

		#region Public Methods

		public int SearchBestMove(byte depth = 4)
		{
			return SearchBestMove(depth, Field.CurrentPlayer, -AiSettings.InfinityScore, AiSettings.InfinityScore);
		}

		public int SearchBestMove(byte depth, Dot player, float alpha, float beta)
		{
			int bestMove = 0;

			var moves = MoveGenerator.GenerateMovesForPlayer(player);
			Dot nextPlayer = player.NextPlayer();

			foreach (var move in moves)
			{
				if (alpha < beta)
				{
					Field.MakeMove(move);
					float tmp = -EvaluatePosition((byte)(depth - 1), nextPlayer, -beta, -alpha);
					Field.UnmakeMove();
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

		private float EvaluatePosition(byte depth, Dot player, float alpha, float beta)
		{
			if (depth == 0)
				return Estimator.Estimate(player);

			Dot nextPlayer = player.NextPlayer();

			var moves = MoveGenerator.GenerateMovesForPlayer(player);

			foreach (var move in moves)
			{
				if (alpha < beta)
				{
					Field.MakeMove(move);
					float tmp = -EvaluatePosition((byte)(depth - 1), nextPlayer, -beta, -alpha);
					Field.UnmakeMove();
					if (tmp > alpha)
						alpha = tmp;
				}
			}

			return alpha;
		}

		#endregion

		#region Properties

		public Field Field
		{
			get;
			set;
		}

		public MoveGenerator MoveGenerator
		{
			get;
			set;
		}

		public Estimator Estimator
		{
			get;
			set;
		}

		#endregion
	}
}
