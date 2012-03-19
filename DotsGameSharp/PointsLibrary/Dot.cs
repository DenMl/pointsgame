using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace PointsLibrary
{
	[Flags]
	public enum Dot
	{
		Player = 1,
		Putted = 2,
		Surrouded = 4,
		Bound = 8,
		EmptyBound = 16,
		EmptyBase = 32,
		Tagged = 64,
		BadValue = 128,

		Empty = 0,
		Red = 0,
		Blue = 1,
	}
}
