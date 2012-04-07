using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Dots.Library;

namespace Dots.AI
{
	public class TranspositionTable
	{
		#region Fields

		private Field Field_;
		
		#endregion

		#region Constructors
		
		public TranspositionTable(Field field)
		{
			Field_ = field;
			HashEntries_ = new HashEntry[AiSettings.HashTableSize];
		}

		#endregion

		#region Public Methods

		public unsafe void RecordHash(byte depth, float score, enmHashEntryType type, ulong key, ushort move)
		{
			fixed (HashEntry* entry = &HashEntries_[key % AiSettings.HashTableSize])
			{
				if (type == enmHashEntryType.Alpha && 
					(entry->Type == enmHashEntryType.Exact || entry->Type == enmHashEntryType.Beta))
					return;

				if (entry->Depth <= depth)
				{
					entry->Depth = depth;
					entry->Score = score;
					entry->Type = type;
					entry->HashKey = key;
					if (type != enmHashEntryType.Alpha)
						entry->BestMove = move;
					else
					{
						entry->BestMove = entry->BestMove;
					}
				}
			}
		}

		public HashEntry[] HashEntries_;

		#endregion

		#region Helpers

		

		#endregion
	}
}
