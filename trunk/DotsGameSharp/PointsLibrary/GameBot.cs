using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Security.Cryptography;
using Dots.Library;

namespace Dots.AI
{
	public class GameBot
	{
		Field[] Field;
		MoveGenerator MoveGenerator;

		public GameBot(Field field)
		{
			Field = new Field[] { field };
			MoveGenerator = new MoveGenerator(field);
		}

		public int Depth;
		public int OldRedCaptureCount;
		public int OldBlueCaprtureCount;

		private int SearchBestMove(int depth, Dot player)
		{
			Depth = depth;

			var score = -100;
			var opponentPlayer = player.NextPlayer();
			int bestMove = 0;

			var tempScore = new List<int>();
			var tempPositions = new List<int>();

			var moves = MoveGenerator.GenerateMovesForPlayer(player);
			foreach (var move in moves)
			{
				Field[0].MakeMove(move);
				var tmp = EvaluatePosition(depth - 1, player);
				Field[0].UnmakeMove();

				tempScore.Add(tmp);
				tempPositions.Add(move);
				if (tmp > score)
				{
					score = tmp;
					bestMove = move;
				} 
			}

			return bestMove;
		}

		private int EvaluatePosition(int depth, Dot player)
		{
			int score;

			score = Field[0].LastMoveCaptureCount + Field[0].LastMoveFreedCount;

			if (score != 0)
			{
			}

			var opponentPlayer = player.NextPlayer();

			if (depth != 0)
			{
				var moves = MoveGenerator.GenerateMovesForPlayer(player);
				foreach (var move in moves)
				{
					Field[0].MakeMove(move);
					var tmp = -EvaluatePosition(depth - 1, opponentPlayer);
					Field[0].UnmakeMove();
					if (Math.Abs(tmp) > Math.Abs(score))
						score = tmp;
				}
			}

			return score;
		}

		public int MakeMove()
		{
			OldRedCaptureCount = Field[0].RedCaptureCount;
			OldBlueCaprtureCount = Field[0].BlueCaptureCount;

			return SearchBestMove(4, Field[0].CurrentPlayer);

			//GenerateTacticalMoves();

			//GenerateStrategicalMoves();

			//Field.MakeMove(position);
		}
	}
}