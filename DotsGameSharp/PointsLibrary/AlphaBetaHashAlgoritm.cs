using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace PointsLibrary
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
					float tmp = -EvaluatePosition((byte)(depth - 1), nextPlayer, -beta, -alpha, 1, HashField_.Key);
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

		private unsafe float EvaluatePosition(byte depth, Dot player, float alpha, float beta, int ply, ulong key)
		{
			float oldAlpha = alpha;
			Dot nextPlayer = player.NextPlayer();

			fixed (HashEntry* hashEntry = &transpositionTable_.HashEntries_[key % TranspositionTable.TableSize])
			{
				if (hashEntry->HashKey == key)
				{
					if (/*ply > 0 && */hashEntry->Depth >= depth)
					{
						float score = hashEntry->Score;
						/*if (score > AiSettings.InfinityScore - AiSettings.MaxPly)
							score -= ply;
						else if (score < -AiSettings.InfinityScore + AiSettings.MaxPly)
							score += ply;*/

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
						float tmp = -EvaluatePosition((byte)(depth - 1), nextPlayer, -beta, -alpha, ply + 1, HashField_.Key);
						Field_.UnmakeMove();
						HashField_.UpdateHash();

						if (tmp > alpha)
						{
							transpositionTable_.RecordHash(depth, tmp, tmp < beta ? enmHashEntryType.Exact : enmHashEntryType.Beta,
								key, hashEntry->BestMove, ply);
							alpha = tmp;
							if (alpha >= beta)
								return beta;
						}
					}
				}
			}

			if (depth == 0)
				return player == Dot.Red ? Field_.RedCaptureCount - Field_.BlueCaptureCount :
					Field_.BlueCaptureCount - Field_.RedCaptureCount;

			var moves = MoveGenerator_.GenerateMovesForPlayer(player);
			foreach (var move in moves)
			{
				Field_.MakeMove(move);
				HashField_.UpdateHash();
				float tmp = -EvaluatePosition((byte)(depth - 1), nextPlayer, -beta, -alpha, ply + 1, HashField_.Key);
				Field_.UnmakeMove();
				HashField_.UpdateHash();

				if (tmp > alpha)
				{
					transpositionTable_.RecordHash(
						(byte)depth, tmp, tmp < beta ? enmHashEntryType.Exact : enmHashEntryType.Beta, HashField_.Key, (ushort)move, ply);
					alpha = tmp;
					if (alpha >= beta)
						return beta;
				}
			}

			if (alpha == oldAlpha)
				transpositionTable_.RecordHash((byte)depth, alpha, enmHashEntryType.Alpha, HashField_.Key, 0, ply);

			return alpha;
		}

		#endregion
	}
}
