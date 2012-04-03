using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Dots.Library
{
	public class Base
	{
		public readonly int CaptureCountRed;
		public readonly int CaptureCountBlue;
		public readonly int RedSquare;
		public readonly int BlueSquare;
		public readonly List<int> ChainPositions;
		public readonly List<int> SurroundPoistions;
		public readonly Stack<DotPosition> ChainDotPositions;
		public readonly Stack<DotPosition> SurrroundDotPositions;
		

		public Base(int captureCountsRed, 
			int captureCountsBlack, 
			Stack<DotPosition> chainPointPoses,
			Stack<DotPosition> surPointPoses,
			List<int> chainPositions,
			List<int> surroundPoistions,
			int redSquare, int blueSquare)
		{
			CaptureCountRed = captureCountsRed;
			CaptureCountBlue = captureCountsBlack;
			ChainDotPositions = chainPointPoses;
			SurrroundDotPositions = surPointPoses;
			ChainPositions = chainPositions;
			SurroundPoistions = surroundPoistions;
			RedSquare = redSquare;
			BlueSquare = blueSquare;
		}

		public override string ToString()
		{
			return "R: " + CaptureCountRed + ", B: " + CaptureCountBlue;
		}
	}
}
