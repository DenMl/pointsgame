using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Dots.Library
{
	public struct PatternDotPosition
	{
		public enmPatternDotState PatternDotState;
		public int Position;
	}

	public class Pattern
	{
		public int Width;
		public int Height;
		public enmPatternDotState[] PatternStates;

		public double Probability;
	}
}
