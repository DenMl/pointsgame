using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Dots.Library;

namespace Dots.AI
{
	class AlphaBetaHashAlgoritm
	{
		#region Constructors

		public AlphaBetaHashAlgoritm(Field field, MoveGenerator moveGenerator = null, Estimator estimator = null,
			ZobristHashField hashField = null, TranspositionTable transpositionTable = null)
		{
			Field = field;
			MoveGenerator = moveGenerator ?? new MoveGenerator(field);
			Estimator = estimator ?? new Estimator(field);
			HashField = hashField ?? new ZobristHashField(field, 0);
			TranspositionTable = transpositionTable ?? new TranspositionTable(field);
		}

		#endregion

		#region Public Methods

		public int SearchBestMove()
		{
			return SearchBestMove(4, Field.CurrentPlayer, -AiSettings.InfinityScore, -AiSettings.InfinityScore);
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
					HashField.UpdateHash();
					float tmp = -EvaluatePosition((byte)(depth - 1), nextPlayer, -beta, -alpha, HashField.Key);
					Field.UnmakeMove();
					HashField.UpdateHash();
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

		private unsafe float EvaluatePosition(byte depth, Dot player, float alpha, float beta, ulong key)
		{
			float oldAlpha = alpha;
			Dot nextPlayer = player.NextPlayer();

			float score = CheckCollision(player, depth, alpha, beta, key);
			if (score >= 0)
				return score;

			if (depth == 0)
				return Estimator.Estimate(player);

			var moves = MoveGenerator.GenerateMovesForPlayer(player);
			foreach (var move in moves)
			{
				Field.MakeMove(move);
				HashField.UpdateHash();
				float tmp = -EvaluatePosition((byte)(depth - 1), nextPlayer, -beta, -alpha, HashField.Key);
				Field.UnmakeMove();
				HashField.UpdateHash();

				if (tmp > alpha)
				{
					TranspositionTable.RecordHash(
						(byte)depth, tmp, tmp < beta ? enmHashEntryType.Exact : enmHashEntryType.Beta, HashField.Key, (ushort)move);
					
					alpha = tmp;
					if (alpha >= beta)
						return beta;
				}
			}

			if (alpha == oldAlpha)
				TranspositionTable.RecordHash((byte)depth, alpha, enmHashEntryType.Alpha, HashField.Key, 0);

			return alpha;
		}

		private unsafe float CheckCollision(Dot player, byte depth, float alpha, float beta, ulong key)
		{
			fixed (HashEntry* hashEntry = &TranspositionTable.HashEntries_[key % TranspositionTable.TableSize])
			{
				if (hashEntry->HashKey == key)
				{
					if (hashEntry->Depth >= depth)
					{
						float score = hashEntry->Score;

						if (hashEntry->Type == enmHashEntryType.Alpha)
						{
							if (score <= alpha)
								return alpha;
						}
						else
						{
							if (score > alpha)
								alpha = score;
							if (alpha >= beta)
								return beta;
						}
					}
					if (hashEntry->Type != enmHashEntryType.Alpha)
					{
						Field.MakeMove(hashEntry->BestMove);
						HashField.UpdateHash();
						float tmp = -EvaluatePosition((byte)(depth - 1), player.NextPlayer(), -beta, -alpha, HashField.Key);
						Field.UnmakeMove();
						HashField.UpdateHash();

						if (tmp > alpha)
						{
							TranspositionTable.RecordHash(depth, tmp, tmp < beta ? enmHashEntryType.Exact : enmHashEntryType.Beta,
								key, hashEntry->BestMove);
							alpha = tmp;
							if (alpha >= beta)
								return beta;
						}
					}
				}
			}
			return -1;
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

		public ZobristHashField HashField
		{
			get;
			set;
		}

		public TranspositionTable TranspositionTable
		{
			get;
			set;
		}

		#endregion
	}
}
