using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Dots.Library;

namespace Dots.AI
{
	class LinkedGroup
	{
		public readonly Dot Player;
		public readonly List<int> Positions;

		private List<int> EnvelopePositions_;

		public LinkedGroup(Dot player, List<int> positions)
		{
			Player = player;
			Positions = positions;
		}

		public void BuildEnvelope()
		{
			EnvelopePositions_ = new List<int>(Positions);

			if (Positions.Count == 1)
				return;

			int minPos = Positions.Min();
			int minPosX = minPos % Field.RealWidth;
			int minPosY = minPos / Field.RealWidth;

			EnvelopePositions_.Remove(minPos);

			EnvelopePositions_.Sort((pos2, pos1) =>
				{
					int x1 = pos1 % Field.RealWidth - minPosX;
					int y1 = pos1 / Field.RealWidth - minPosY;
					int x2 = pos2 % Field.RealWidth - minPosX;
					int y2 = pos2 / Field.RealWidth - minPosY;
					return ((float)x1 / (Math.Abs(x1) + Math.Abs(y1))).CompareTo((float)x2 / (Math.Abs(x2) + Math.Abs(y2)));
				});

			EnvelopePositions_.Insert(0, minPos);

			int m = 0;
			for (int i = 1; i < EnvelopePositions_.Count; i++)
			{
				if (i != m)
				{
					if (m > 1)
					{
						while (m >= 1 && IsCCW(EnvelopePositions_[m - 1], EnvelopePositions_[m], EnvelopePositions_[i]) <= 0)
							m--;
					}
				}
				m++;
				int t = EnvelopePositions_[m];
				EnvelopePositions_[m] = EnvelopePositions_[i];
				EnvelopePositions_[i] = t;
			}

			if (m + 1 != EnvelopePositions_.Count)
				EnvelopePositions_.RemoveRange(EnvelopePositions_.Count - m - 1, m + 1);
		}
		
		private float IsCCW(int p1, int p2, int p3)
		{
			return (p2 % Field.RealWidth - p1 % Field.RealWidth) *
				(p3 / Field.RealWidth - p1 / Field.RealWidth) -
				(p2 / Field.RealWidth - p1 / Field.RealWidth) *
				(p3 % Field.RealWidth - p1 % Field.RealWidth);
		}

		public IEnumerable<int> EnvelopePositions
		{
			get
			{
				return EnvelopePositions_;
			}
		}
	}
}
