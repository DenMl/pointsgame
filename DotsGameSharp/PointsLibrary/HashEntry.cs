using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace Dots.AI
{
	enum enmHashEntryType : byte
	{
		Empty = 0,
		Exact = 1,
		Alpha = 2,
		Beta = 3
	}

	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	struct HashEntry
	{
		public byte Depth;

		public enmHashEntryType Type;

		public ushort BestMove;

		public float Score;

		public ulong HashKey;
	}
}
