using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace PointsLibrary
{
	public static class AiSettings
	{
		public static float InfinityScore
		{
			get;
			private set;
		}

		public static float MaxPly
		{
			get;
			private set;
		}

		static AiSettings()
		{
			InfinityScore = 100;
			MaxPly = 50;
		}
	}
}
