using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Dots.Library;

namespace Dots.AI
{
	class StrategicMovesAnalyzer
	{
		#region Fields

		private Field Field_;
		private List<LinkedGroup> RedGroups_;
		private List<LinkedGroup> BlueGroups_;

		#endregion

		#region Constructors

		public StrategicMovesAnalyzer(Field field)
		{
			Field_ = field;
		}

		#endregion

		#region Public Methods

		#endregion

		#region Helpers

		public void GenerateGroups()
		{
			RedGroups_ = new List<LinkedGroup>();
			BlueGroups_ = new List<LinkedGroup>();
			for (int i = 1; i <= Field_.Width; i++)
				for (int j = 1; j <= Field_.Height; j++)
				{
					int ind = j * Field.RealWidth + i;

					if (Field_[ind].IsRedPutted() && !Field_[ind].IsTagged())
						RedGroups_.Add(new LinkedGroup(Dot.RedPlayer, FillDiagVertHorizLinkedDots(ind)));
					else if (Field_[ind].IsBluePutted() && !Field_[ind].IsTagged())
						BlueGroups_.Add(new LinkedGroup(Dot.BluePlayer, FillDiagVertHorizLinkedDots(ind)));
				}
		}

		private List<int> FillDiagVertHorizLinkedDots(int pos)
		{
			var result = new List<int>();

			Dot player = Field_[pos] & Dot.Player;
			var tempStack = new Stack<int>();
			tempStack.Push(pos);
			result.Add(pos);

			List<Dot> dots = new List<Dot>();
			while (tempStack.Count != 0)
			{
				pos = tempStack.Pop();
				result.Add(pos);

				if (Field_[pos - 1].IsPlayerPutted(player) && !Field_[pos - 1].IsTagged())
				{
					tempStack.Push(pos - 1);
					Field_[pos - 1] |= Dot.Tagged;
				}

				if (Field_[pos - Field.RealWidth - 1].IsPlayerPutted(player) && !Field_[pos - Field.RealWidth - 1].IsTagged())
				{
					tempStack.Push(pos - Field.RealWidth - 1);
					Field_[pos - Field.RealWidth - 1] |= Dot.Tagged;
				}

				if (Field_[pos - Field.RealWidth].IsPlayerPutted(player) && !Field_[pos - Field.RealWidth].IsTagged())
				{
					tempStack.Push(pos - Field.RealWidth);
					Field_[pos - Field.RealWidth] |= Dot.Tagged;
				}

				if (Field_[pos - Field.RealWidth + 1].IsPlayerPutted(player) && !Field_[pos - Field.RealWidth + 1].IsTagged())
				{
					tempStack.Push(pos - Field.RealWidth + 1);
					Field_[pos - Field.RealWidth + 1] |= Dot.Tagged;
				}

				if (Field_[pos + 1].IsPlayerPutted(player) && !Field_[pos + 1].IsTagged())
				{
					tempStack.Push(pos + 1);
					Field_[pos + 1] |= Dot.Tagged;
				}

				if (Field_[pos + Field.RealWidth + 1].IsPlayerPutted(player) && !Field_[pos + Field.RealWidth + 1].IsTagged())
				{
					tempStack.Push(pos + Field.RealWidth + 1);
					Field_[pos + Field.RealWidth + 1] |= Dot.Tagged;
				}

				if (Field_[pos + Field.RealWidth].IsPlayerPutted(player) && !Field_[pos + Field.RealWidth].IsTagged())
				{
					tempStack.Push(pos + Field.RealWidth);
					Field_[pos + Field.RealWidth] |= Dot.Tagged;
				}

				if (Field_[pos + Field.RealWidth - 1].IsPlayerPutted(player) && !Field_[pos + Field.RealWidth - 1].IsTagged())
				{
					tempStack.Push(pos + Field.RealWidth - 1);
					Field_[pos + Field.RealWidth - 1] |= Dot.Tagged;
				}
			}

			return result;
		}

		private List<int> FillVertHorizLinkedDots(int pos)
		{
			var result = new List<int>();

			Dot player = Field_[pos] & Dot.Player;
			var tempStack = new Stack<int>();
			tempStack.Push(pos);
			result.Add(pos);

			List<Dot> dots = new List<Dot>();
			while (tempStack.Count != 0)
			{
				pos = tempStack.Pop();
				result.Add(pos);

				if (Field_[pos - 1].IsPlayerPutted(player) && !Field_[pos - 1].IsTagged())
				{
					tempStack.Push(pos - 1);
					Field_[pos - 1] |= Dot.Tagged;
				}

				if (Field_[pos - Field.RealWidth].IsPlayerPutted(player) && !Field_[pos - Field.RealWidth].IsTagged())
				{
					tempStack.Push(pos - Field.RealWidth);
					Field_[pos - Field.RealWidth] |= Dot.Tagged;
				}

				if (Field_[pos + 1].IsPlayerPutted(player) && !Field_[pos + 1].IsTagged())
				{
					tempStack.Push(pos + 1);
					Field_[pos + 1] |= Dot.Tagged;
				}

				if (Field_[pos + Field.RealWidth].IsPlayerPutted(player) && !Field_[pos + Field.RealWidth].IsTagged())
				{
					tempStack.Push(pos + Field.RealWidth);
					Field_[pos + Field.RealWidth] |= Dot.Tagged;
				}
			}

			return result;
		}

		private void ClearAllTags()
		{
			for (int i = 1; i <= Field_.Width; i++)
				for (int j = 1; j <= Field_.Height; j++)
				{
					Field_[j * Field.RealWidth + i].ClearTag();
				}
		}

		#endregion

		#region Properties

		public IEnumerable<LinkedGroup> RedGroups
		{
			get
			{
				return RedGroups_;
			}
		}

		public IEnumerable<LinkedGroup> BlueGroups
		{
			get
			{
				return BlueGroups_;
			}
		}

		#endregion
	}
}
