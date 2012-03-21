using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Dots.Library;

namespace Dots.AI
{
	class AlphaBetaHashAlgoritm
	{
		#region Fields

		private Field Field_;
		private MoveGenerator MoveGenerator_;
		private ZobristHashField HashField_;
		private TranspositionTable transpositionTable_;

		#endregion

		#region Constructors

		public AlphaBetaHashAlgoritm(Field field)
		{
			Field_ = field;
			MoveGenerator_ = new MoveGenerator(field);
			HashField_ = new ZobristHashField(field, 0);
			transpositionTable_ = new TranspositionTable(field);
		}

		public AlphaBetaHashAlgoritm(Field field, ZobristHashField hashField, TranspositionTable transpositionTable)
		{
			Field_ = field;
			MoveGenerator_ = new MoveGenerator(field);
			HashField_ = hashField;
			transpositionTable_ = transpositionTable;
		}

		#endregion

		#region Public Methods

		public int SearchBestMove()
		{
			return SearchBestMove(4, Field_.CurrentPlayer, -AiSettings.InfinityScore, -AiSettings.InfinityScore);
		}

		public int SearchBestMove(byte depth, Dot player, float alpha, float beta)
		{
			int bestMove = 0;

			var moves = MoveGenerator_.GenerateMovesForPlayer(player);
			Dot nextPlayer = player.NextPlayer();

			foreach (var move in moves)
			{
				if (alpha < beta)
				{
					Field_.MakeMove(move);
					HashField_.UpdateHash();
					float tmp = -EvaluatePosition((byte)(depth - 1), nextPlayer, -beta, -alpha, HashField_.Key);
					Field_.UnmakeMove();
					HashField_.UpdateHash();
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
			{
				score = player == Dot.RedPlayer ? Field_.RedCaptureCount - Field_.BlueCaptureCount :
					Field_.BlueCaptureCount - Field_.RedCaptureCount;

				return score;
			}

			var moves = MoveGenerator_.GenerateMovesForPlayer(player);
			foreach (var move in moves)
			{
				Field_.MakeMove(move);
				HashField_.UpdateHash();
				float tmp = -EvaluatePosition((byte)(depth - 1), nextPlayer, -beta, -alpha, HashField_.Key);
				Field_.UnmakeMove();
				HashField_.UpdateHash();

				if (tmp > alpha)
				{
					transpositionTable_.RecordHash(
						(byte)depth, tmp, tmp < beta ? enmHashEntryType.Exact : enmHashEntryType.Beta, HashField_.Key, (ushort)move);
					
					alpha = tmp;
					if (alpha >= beta)
						return beta;
				}
			}

			if (alpha == oldAlpha)
				transpositionTable_.RecordHash((byte)depth, alpha, enmHashEntryType.Alpha, HashField_.Key, 0);

			return alpha;
		}

		private unsafe float CheckCollision(Dot player, byte depth, float alpha, float beta, ulong key)
		{
			fixed (HashEntry* hashEntry = &transpositionTable_.HashEntries_[key % TranspositionTable.TableSize])
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
						Field_.MakeMove(hashEntry->BestMove);
						HashField_.UpdateHash();
						float tmp = -EvaluatePosition((byte)(depth - 1), player.NextPlayer(), -beta, -alpha, HashField_.Key);
						Field_.UnmakeMove();
						HashField_.UpdateHash();

						if (tmp > alpha)
						{
							transpositionTable_.RecordHash(depth, tmp, tmp < beta ? enmHashEntryType.Exact : enmHashEntryType.Beta,
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
	}
}
