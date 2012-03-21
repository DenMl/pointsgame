using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Dots.Library;

namespace Dots.AI
{
	class TranspositionTable
	{
		#region Constants

		public const int TableSize = 1 << 20;

		#endregion

		#region Fields

		private Field Field_;
		
		#endregion

		#region Constructors
		
		public TranspositionTable(Field field)
		{
			Field_ = field;
			HashEntries_ = new HashEntry[TableSize];
		}

		#endregion

		#region Public Methods

		public unsafe void RecordHash(byte depth, float score, enmHashEntryType type, ulong key, ushort move)
		{
			fixed (HashEntry* entry = &HashEntries_[key % TableSize])
			{
				if (type == enmHashEntryType.Alpha && entry->Type == enmHashEntryType.Beta)
					return;

				if (entry->Depth <= depth)
				{
					entry->Depth = depth;
					entry->Score = score;
					entry->Type = type;
					entry->HashKey = key;
					if (type != enmHashEntryType.Alpha)
						entry->BestMove = move;
				}
			}
		}

		public HashEntry[] HashEntries_;

		#endregion

		#region Helpers

		

		#endregion
	}
}
