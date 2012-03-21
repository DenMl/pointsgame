using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Dots.Library
{
	public sealed class Field
	{
		#region Constatnts and masks

		public const int RealWidth = 64;


		#endregion

		#region Fields

		private Stack<int> TempStack_;
		private List<int> ChainPositions_;
		private List<int> SurroundPositions_;
		private bool EmptyBaseCreated_;
		private int RedCaptureCount_;
		private int BlueCaptureCount_;
		private Dot CurrentPlayer_;
		private int LastPosition_;
		private int LastBaseCaptureCount_;
		private int LastBaseFreedCount_;
		private int LastMoveCaptureCount_;
		private int LastMoveFreedCount_;
		private enmMoveState LastMoveState_;

		/// <summary>
		/// Indexes and states of last move chain dots.
		/// </summary>
		private Stack<DotPosition> ChainDotsPositions_;

		/// <summary>
		/// Indexes and states of last move surorund dots.
		/// </summary>
		private Stack<DotPosition> SurroundDotsPositions_;

		/// <summary>
		/// Array of dots or dot states
		/// </summary>
		private Dot[] Dots_;

		/// <summary>
		/// Sequance of putted dots.
		/// Using for moves rollback.
		/// </summary>
		private List<State> DotsSequanceStates_;

		private List<int> InputChainDots_;

		private List<int> InputSurroundedDots_;

		#endregion

		#region Readonly

		public readonly int Width;

		public readonly int Height;

		public readonly enmSurroundCondition SurroundCondition;

		#endregion

		#region Main Properties

		public Dot this[int i]
		{
			get
			{
				return Dots_[i];
			}
		}

		public IEnumerable<State> DotsSequanceStates
		{
			get
			{
				return DotsSequanceStates_;
			}
		}

		public int RedCaptureCount
		{
			get
			{
				return RedCaptureCount_;
			}
		}

		public int BlueCaptureCount
		{
			get
			{
				return BlueCaptureCount_;
			}
		}

		public enmMoveState LastMoveState
		{
			get
			{
				return LastMoveState_;
			}
		}

		public int RealDotsCount
		{
			get
			{
				return Dots_.Length;
			}
		}

		public int DotsSequenceCount
		{
			get
			{
				return DotsSequanceStates_.Count;
			}
		}

		#endregion

		#region Last Move

		public Dot CurrentPlayer
		{
			get
			{
				return CurrentPlayer_;
			}
		}

		public int LastPosition
		{
			get
			{
				return LastPosition_;
			}
		}

		public int LastMoveCaptureCount
		{
			get
			{
				return LastMoveCaptureCount_;
			}
		}

		public int LastMoveFreedCount
		{
			get
			{
				return LastMoveFreedCount_;
			}
		}

		/// <summary>
		/// Indexes of last move chain dots.
		/// </summary>
		public IEnumerable<int> ChainPositions
		{
			get
			{
				return ChainPositions_;
			}
		}

		/// <summary>
		/// Indexes of last move surorund dots.
		/// </summary>
		public IEnumerable<int> SurroundPositions
		{
			get
			{
				return SurroundPositions_;
			}
		}

		#endregion

		#region Constructors

		public Field()
			: this(39, 32, enmSurroundCondition.Standart)
		{
		}

		public Field(int width, int height, enmSurroundCondition surroundCondition)
		{
			Width = width;
			Height = height;
			SurroundCondition = surroundCondition;

			Dots_ = new Dot[RealWidth * (height + 2)];
			FillBadValues();

			DotsSequanceStates_ = new List<State>(64);

			ChainPositions_ = new List<int>(16);
			SurroundPositions_ = new List<int>(4);
			ChainDotsPositions_ = new Stack<DotPosition>(16);
			SurroundDotsPositions_ = new Stack<DotPosition>(4);
			TempStack_ = new Stack<int>(16);
			InputChainDots_ = new List<int>(4);
			InputSurroundedDots_ = new List<int>(4);
			LastMoveState_ = enmMoveState.None;
		}

		#endregion

		#region Private and protected methods

		private static void Swap(ref int pos1, ref int pos2)
		{
			var t = pos1;
			pos1 = pos2;
			pos2 = t;
		}

		/// <summary>
		/// Returns square bounded by the triangle with vertexes (0, pos1, pos2)
		/// </summary>
		/// <param name="pos1"></param>
		/// <param name="pos2"></param>
		/// <returns></returns>
		private static int Square(int pos1, int pos2)
		{
			return (pos1 % RealWidth) * (pos2 / RealWidth) - (pos1 / RealWidth) * (pos2 % RealWidth);
		}

		/// <summary>
		///  * . .   x . *   . x x   . . .
		///  . o .   x o .   . o .   . o x
		///  x x .   . . .   . . *   * . x
		///  o - center pos
		///  x - pos
		///  * - result (new pos)
		/// </summary>
		/// <param name="centerPosition"></param>
		/// <param name="pos"></param>
		private static void GetFirstNextPos(int centerPosition, ref int pos)
		{
			if (pos < centerPosition)
			{
				if ((pos == centerPosition - RealWidth - 1) || (pos == centerPosition - 1))
					pos = centerPosition - RealWidth + 1;
				else
					pos = centerPosition + RealWidth + 1;
			}
			else
			{
				if ((pos == centerPosition + 1) || (pos == centerPosition + RealWidth + 1))
					pos = centerPosition + RealWidth - 1;
				else
					pos = centerPosition - RealWidth - 1;
			}
		}

		private void RemoveEmptyBaseFlags(int startPosition)
		{
			SurroundDotsPositions_.Push(new DotPosition(startPosition, Dots_[startPosition]));
			Dots_[startPosition] &= ~Dot.EmptyBase;

			var pos = startPosition;
			TempStack_.Clear();
			TempStack_.Push(startPosition);

			while (TempStack_.Count != 0)
			{
				pos = TempStack_.Pop();

				if ((Dots_[pos - 1] & Dot.EmptyBase) == Dot.EmptyBase)
				{
					// Save state for rollback.
					SurroundDotsPositions_.Push(new DotPosition(pos - 1, Dots_[pos - 1]));

					TempStack_.Push(pos - 1);
					Dots_[pos - 1] &= ~Dot.EmptyBase;
				}

				if ((Dots_[pos - RealWidth] & Dot.EmptyBase) == Dot.EmptyBase)
				{
					// Save state for rollback.
					SurroundDotsPositions_.Push(new DotPosition(pos - RealWidth, Dots_[pos - RealWidth]));

					TempStack_.Push(pos - RealWidth);
					Dots_[pos - RealWidth] &= ~Dot.EmptyBase;
				}

				if ((Dots_[pos + 1] & Dot.EmptyBase) == Dot.EmptyBase)
				{
					// Save state for rollback.
					SurroundDotsPositions_.Push(new DotPosition(pos + 1, Dots_[pos + 1]));

					TempStack_.Push(pos + 1);
					Dots_[pos + 1] &= ~Dot.EmptyBase;
				}

				if ((Dots_[pos + RealWidth] & Dot.EmptyBase) == Dot.EmptyBase)
				{
					// Save state for rollback.
					SurroundDotsPositions_.Push(new DotPosition(pos + RealWidth, Dots_[pos + RealWidth]));

					TempStack_.Push(pos + RealWidth);
					Dots_[pos + RealWidth] &= ~Dot.EmptyBase;
				}
			}
		}

		private void SubCapturedFreedCount(Dot dotColor)
		{
			if (dotColor == 0)
			{
				if (LastMoveCaptureCount_ > 0)
				{
					RedCaptureCount_ -= LastMoveCaptureCount_;
					BlueCaptureCount_ += LastMoveFreedCount_;
				}
				else
					BlueCaptureCount_ += LastMoveCaptureCount_;
			}
			else
			{
				if (LastMoveCaptureCount_ > 0)
				{
					BlueCaptureCount_ -= LastMoveCaptureCount_;
					RedCaptureCount_ += LastMoveFreedCount_;
				}
				else
					RedCaptureCount_ += LastMoveCaptureCount_;
			}
		}

		/// <summary>
		/// Check closure at last position.
		/// </summary>
		private void CheckClosure()
		{
			// If putted in empty base.
			if ((Dots_[LastPosition_] & Dot.EmptyBase) == Dot.EmptyBase)
			{
				// Check player of territoty.
				var chainPosition = LastPosition_ - 1;
				while (!Dots_[chainPosition].IsPutted())
					chainPosition--;

				// If put in own empty base.
				if ((Dots_[chainPosition] & Dot.Player) == (Dots_[LastPosition_] & Dot.Player))
				{
					// Подумать, т.к. здесь могут быть сложности.
					Dots_[LastPosition_] &= ~Dot.EmptyBase;
					return;
				}

				if (SurroundCondition == enmSurroundCondition.Standart)
				{
					if (GetInputDots(LastPosition_) > 1)
						FindAndMarkChainAndSurroundedDots(LastPosition_);
					// If surround anything.
					if (ChainPositions_.Count != 0)
					{
						RemoveEmptyBaseFlags(LastPosition_);
						return;
					}
				}

				// Find first opponent dot.
				chainPosition++;
				var opponentEnableCondition = Dot.Putted | CurrentPlayer_.NextPlayer();
				var nextPlayer = CurrentPlayer_.NextPlayer();
				do
				{
					chainPosition--;
					while (!IsEnable(Dots_[chainPosition], opponentEnableCondition))
						chainPosition--;

					int inputChainDot, inputSurroundedDot;
					if (GetInputDotForEmptyBase(chainPosition, nextPlayer, out inputChainDot, out inputSurroundedDot))
					{
						InputChainDots_.Clear();
						InputChainDots_.Add(inputChainDot);
						InputSurroundedDots_.Clear();
						InputSurroundedDots_.Add(inputSurroundedDot);

						ChainPositions_.Clear();
						SurroundPositions_.Clear();
						ChainDotsPositions_.Clear();
						SurroundDotsPositions_.Clear();

						FindAndMarkChainAndSurroundedDots(chainPosition);
					}
				}
				while (Dots_[LastPosition_].IsZeroSurroundLevel());

				LastMoveCaptureCount_ = -1;
			}
			else
			{
				if (GetInputDots(LastPosition_) > 1)
					FindAndMarkChainAndSurroundedDots(LastPosition_);
			}
		}

		/// <summary>
		/// Highly-optimized function for finding and disabling closed territory and its bound.
		/// </summary>
		/// <param name="position">Starting position of Dot in one-dimensional array</param>
		private void FindAndMarkChainAndSurroundedDots(int position)
		{
			int pos;
			var inputDotsCount = InputChainDots_.Count;

			var dotColor = Dots_[position] & Dot.Player;
			var negativeSquare = true;
			var enabledCondition = GetEnabledCondition(Dots_[position]);

			for (var i = 0; i < InputChainDots_.Count; i++)
			{
				var previousChainDotsCount = ChainPositions_.Count;
				ChainPositions_.Add(position);
				pos = InputChainDots_[i];
				var centerPos = position;

				// Returns square bounded by the triangle with vertexes (0, centerPos, pos)
				int tempSquare = Square(centerPos, pos);
				do
				{
					if ((ChainPositions_.Count > previousChainDotsCount + 1) && (pos == ChainPositions_[ChainPositions_.Count - 2]))
						ChainPositions_.RemoveAt(ChainPositions_.Count - 1);
					else
						ChainPositions_.Add(pos);
					Swap(ref pos, ref centerPos);

					GetFirstNextPos(centerPos, ref pos);
					while (!IsEnable(Dots_[pos], enabledCondition))
						GetNextPos(centerPos, ref pos);

					tempSquare += Square(centerPos, pos);
				}
				while (pos != position);

				// Bypass territory only couter-clockwise (territiry wtih negative square)
				if (tempSquare >= 0)
				{
					ChainPositions_.RemoveRange(previousChainDotsCount, ChainPositions_.Count - previousChainDotsCount);
					negativeSquare = false;
				}
				else
				{
					for (var j = previousChainDotsCount; j < ChainPositions_.Count; j++)
					{
						// Save chain dot states for further rollback.
						ChainDotsPositions_.Push(new DotPosition(ChainPositions_[j], Dots_[ChainPositions_[j]]));

						// Mark surrounded dots by flag "Bound".
						Dots_[ChainPositions_[j]] |= Dot.Bound;
					}

					var previousSuroundDotsCount = SurroundPositions_.Count;

					// Find array of dots, bounded by ChainPositions
					AddCapturedDots(InputSurroundedDots_[i], dotColor);

					// Changing "RedCapturedCount" and "BlackCaptureCount".
					AddCapturedFreedCount(dotColor);

					// If capture not empty base or turned on an option "Surround all".
					if ((LastMoveCaptureCount_ != 0) || (SurroundCondition == enmSurroundCondition.Always))
					{
						for (var j = previousSuroundDotsCount; j < SurroundPositions_.Count; j++)
						{
							// Clear tag.
							Dots_[SurroundPositions_[j]] = Dots_[SurroundPositions_[j]] & ~Dot.Tagged;

							// Save surrounded dot states for further rollback.
							SurroundDotsPositions_.Push(new DotPosition(SurroundPositions_[j], Dots_[SurroundPositions_[j]]));

							SetCaptureFreeState(SurroundPositions_[j], dotColor);
						}
					}
					// If capture empty base.
					else
					{
						for (var j = previousChainDotsCount; j < ChainPositions_.Count; j++)
						{
							// Clear "Bound" flag and set special flag "EmptyBound".
							Dots_[ChainPositions_[j]] &= ~Dot.Bound;
							Dots_[ChainPositions_[j]] |= Dot.EmptyBound;
						}

						for (var j = previousSuroundDotsCount; j < SurroundPositions_.Count; j++)
						{
							// Clear tag.
							Dots_[SurroundPositions_[j]] &= ~Dot.Tagged;

							// Save surrounded dot states for further rollback.
							SurroundDotsPositions_.Push(new DotPosition(SurroundPositions_[j], Dots_[SurroundPositions_[j]]));

							// If dot is not putted in empty territory then set flag "EmptyBase"
							if ((Dots_[SurroundPositions_[j]] & Dot.Putted) == 0)
								Dots_[SurroundPositions_[j]] |= Dot.EmptyBase;
						}

						ChainPositions_.RemoveRange(previousChainDotsCount, ChainPositions_.Count - previousChainDotsCount);
						SurroundPositions_.RemoveRange(previousSuroundDotsCount, SurroundPositions_.Count - previousSuroundDotsCount);
					}

					// Special optimization.
					if ((i == InputChainDots_.Count - 2) && negativeSquare)
						break;
				}
			}
		}

		private void AddCapturedDots(int startPosition, Dot player)
		{
			Dot boundCondition = player | Dot.Putted | Dot.Bound;

			Dots_[startPosition] |= Dot.Tagged;

			LastBaseCaptureCount_ = 0;
			LastBaseFreedCount_ = 0;
			var pos = startPosition;
			TempStack_.Clear();
			TempStack_.Push(startPosition);

			List<Dot> dots = new List<Dot>();
			while (TempStack_.Count != 0)
			{
				pos = TempStack_.Pop();
				CheckCapturedAndFreed(pos, player);

				SurroundPositions_.Add(pos);
				dots.Add(Dots_[pos]);

				if (!IsBound(Dots_[pos - 1], boundCondition) && !IsTagged(Dots_[pos - 1]))
				{
					TempStack_.Push(pos - 1);
					Dots_[pos - 1] |= Dot.Tagged;
				}

				if (!IsBound(Dots_[pos - RealWidth], boundCondition) && !IsTagged(Dots_[pos - RealWidth]))
				{
					TempStack_.Push(pos - RealWidth);
					Dots_[pos - RealWidth] |= Dot.Tagged;
				}

				if (!IsBound(Dots_[pos + 1], boundCondition) && !IsTagged(Dots_[pos + 1]))
				{
					TempStack_.Push(pos + 1);
					Dots_[pos + 1] |= Dot.Tagged;
				}

				if (!IsBound(Dots_[pos + RealWidth], boundCondition) && !IsTagged(Dots_[pos + RealWidth]))
				{
					TempStack_.Push(pos + RealWidth);
					Dots_[pos + RealWidth] |= Dot.Tagged;
				}
			}
		}

		private void CheckCapturedAndFreed(int position, Dot player)
		{
			if ((Dots_[position] & Dot.RealPutted) == Dot.RealPutted)
			{
				if ((Dots_[position] & Dot.RealPlayer) != (Dot)((int)player << DotConstants.RealPlayerShift))
				{
					LastBaseCaptureCount_++;
					LastMoveCaptureCount_++;
				}
				else if ((int)(Dots_[position] & Dot.SurroundCountMask) >= (int)Dot.FirstSurroundLevel)
				{
					LastBaseFreedCount_++;
					LastMoveFreedCount_++;
				}
			}
		}

		private void SetCaptureFreeState(int pos, Dot player)
		{
			Dots_[pos] |= (Dot)((int)Dots_[pos] << DotConstants.RealPlayerShift) & (Dot.RealPlayer | Dot.RealPutted);
			Dots_[pos] &= ~Dot.Player;
			Dots_[pos] |= Dot.Putted | player | ((Dots_[pos] & Dot.SurroundCountMask) + (int)Dot.FirstSurroundLevel);
		}

		private void AddCapturedFreedCount(Dot dotColor)
		{
			if (dotColor == 0)
			{
				RedCaptureCount_ += LastBaseCaptureCount_;
				BlueCaptureCount_ -= LastBaseFreedCount_;
			}
			else
			{
				BlueCaptureCount_ += LastBaseCaptureCount_;
				RedCaptureCount_ -= LastBaseFreedCount_;
			}
		}

		/// <summary>
		///  . . .   * . .   x * .   . x *   . . x   . . .   . . .   . . .
		///  * o .   x o .   . o .   . o .   . o *   . o x   . o .   . o .
		///  x . .   . . .   . . .   . . .   . . .   . . *   . * x   * x .
		///  o - center pos
		///  x - pos
		///  * - result (new pos)
		/// </summary>
		/// <param name="centerPosition"></param>
		/// <param name="pos"></param>
		private static void GetNextPos(int centerPosition, ref int pos)
		{
			int t;
			if (pos < centerPosition)
			{
				t = centerPosition - RealWidth;
				if (pos == t - 1)
					pos = t;
				else
					if (pos == t)
						pos = t + 1;
					else
						if (pos == t + 1)
							pos = centerPosition + 1;
						else
							pos = t - 1;
			}
			else
			{
				t = centerPosition + RealWidth;
				if (pos == centerPosition + 1)
					pos = t + 1;
				else
					if (pos == t + 1)
						pos = t;
					else
						if (pos == t)
							pos = t - 1;
						else
							pos = centerPosition - 1;
			}
		}

		private int GetInputDots(int centerPos)
		{
			InputChainDots_.Clear();
			InputSurroundedDots_.Clear();

			Dot enableCond = Dots_[centerPos] & Dot.EnableMask;
			if (!IsEnable(Dots_[centerPos - 1], enableCond))
			{
				if (IsEnable(Dots_[centerPos - RealWidth - 1], enableCond))
				{
					InputChainDots_.Add(centerPos - RealWidth - 1);
					InputSurroundedDots_.Add(centerPos - 1);
				}
				else if (IsEnable(Dots_[centerPos - RealWidth], enableCond))
				{
					InputChainDots_.Add(centerPos - RealWidth);
					InputSurroundedDots_.Add(centerPos - 1);
				}
			}

			if (!IsEnable(Dots_[centerPos + RealWidth], enableCond))
			{
				if (IsEnable(Dots_[centerPos + RealWidth - 1], enableCond))
				{
					InputChainDots_.Add(centerPos + RealWidth - 1);
					InputSurroundedDots_.Add(centerPos + RealWidth);

				}
				else if (IsEnable(Dots_[centerPos - 1], enableCond))
				{
					InputChainDots_.Add(centerPos - 1);
					InputSurroundedDots_.Add(centerPos + RealWidth);
				}
			}

			if (!IsEnable(Dots_[centerPos + 1], enableCond))
			{
				if (IsEnable(Dots_[centerPos + RealWidth + 1], enableCond))
				{
					InputChainDots_.Add(centerPos + RealWidth + 1);
					InputSurroundedDots_.Add(centerPos + 1);
				}
				else if (IsEnable(Dots_[centerPos + RealWidth], enableCond))
				{
					InputChainDots_.Add(centerPos + RealWidth);
					InputSurroundedDots_.Add(centerPos + 1);
				}
			}

			if (!IsEnable(Dots_[centerPos - RealWidth], enableCond))
			{
				if (IsEnable(Dots_[centerPos - RealWidth + 1], enableCond))
				{
					InputChainDots_.Add(centerPos - RealWidth + 1);
					InputSurroundedDots_.Add(centerPos - RealWidth);
				}
				else if (IsEnable(Dots_[centerPos + 1], enableCond))
				{
					InputChainDots_.Add(centerPos + 1);
					InputSurroundedDots_.Add(centerPos - RealWidth);
				}
			}

			return InputChainDots_.Count();
		}

		private bool GetInputDotForEmptyBase(int centerPos, Dot Player, out int inputChainDot, out int inputSurroundedDot)
		{
			if (IsEnableForEmptyBase(Dots_[centerPos + 1], Player))
			{
				inputChainDot = 0;
				inputSurroundedDot = 0;
				return false;
			}

			inputSurroundedDot = centerPos + 1;
			var pos = inputSurroundedDot;
			GetNextPos(centerPos, ref pos);
			var k = 0;
			while (!IsEnableForEmptyBase(Dots_[pos], Player) && (k < 8))
			{
				GetNextPos(centerPos, ref pos);
				k++;
			}
			inputChainDot = pos;

			return k != 8;
		}

		private void FillBadValues()
		{
			for (int i = 0; i < RealWidth; i++)
				Dots_[i] = Dot.Invalid;

			for (int i = 1; i < Height + 1; i++)
			{
				Dots_[i * RealWidth] = Dot.Invalid;
				for (int j = Width + 1; j < RealWidth; j++)
					Dots_[i * RealWidth + j] = Dot.Invalid;
			}

			for (int i = Dots_.Length - RealWidth; i < Dots_.Length; i++)
				Dots_[i] = Dot.Invalid;
		}

		#endregion

		#region Public methods

		public static bool IsEnable(Dot dot, Dot enableCondition)
		{
			return (dot & Dot.EnableMask) == enableCondition;
		}

		public static bool IsEnableForEmptyBase(Dot dot, Dot player)
		{
			/*return (dot & Dot.EnableMask) == (Dot.Putted | Player) ||
				(dot & Dot.EnableMask) == (Dot.Putted | Dot.Surrouded | Player.NextPlayer()) ||
				(dot & Dot.EnableMask) == (Dot.Surrouded | Player); // Empty position in Player base*/
			return dot.IsPlayerPutted(player);
		}

		public static bool IsBound(Dot dot, Dot boundCond)
		{
			return (dot & Dot.BoundMask) == boundCond;
		}

		public static bool IsTagged(Dot dot)
		{
			return (dot & Dot.Tagged) == Dot.Tagged;
		}

		public static bool IsPutted(Dot dot)
		{
			return (dot & Dot.Putted) == Dot.Putted;
		}

		public static Dot GetEnabledCondition(Dot dot)
		{
			return dot & Dot.EnableMask;
		}

		public bool IsBaseAddedAtLastMove
		{
			get
			{
				return ChainDotsPositions_.Count != 0 && ChainDotsPositions_.Count != 0 && !EmptyBaseCreated_;
			}
		}

		public static void GetPosition(int position, out int x, out int y)
		{
			x = position % RealWidth - 1;
			y = position / RealWidth - 1;
		}

		public static int GetPosition(int x, int y)
		{
			return (y + 1) * RealWidth + (x + 1);
		}

		public bool MakeMove(int x, int y)
		{
			return MakeMove((y + 1) * RealWidth + (x + 1));
		}

		public bool MakeMove(int position, Dot color)
		{
			Dot oldCurrentPlayer = CurrentPlayer_;
			CurrentPlayer_ = color;
			if (MakeMove(position))
				return true;
			else
			{
				CurrentPlayer_ = oldCurrentPlayer;
				return false;
			}
		}

		public bool MakeMove(int position)
		{
			if (Dots_[position].IsPuttingAllowed())
			{
				Dot oldDot = Dots_[position];

				Dots_[position] |= Dot.Putted;
				Dots_[position] |= CurrentPlayer_;
				Dots_[position] |= Dot.RealPutted;
				Dots_[position] |= (Dot)((int)CurrentPlayer_ << DotConstants.RealPlayerShift);
				LastPosition_ = position;

				ChainPositions_.Clear();
				SurroundPositions_.Clear();
				ChainDotsPositions_.Clear();
				SurroundDotsPositions_.Clear();
				LastMoveCaptureCount_ = 0;
				LastMoveFreedCount_ = 0;

				CheckClosure();

				// Save current state for rollback.
				DotsSequanceStates_.Add(new State()
					{
						Move = new DotPosition(position, oldDot),
						Base = ChainDotsPositions_.Count == 0 ? null :
							new Base(LastMoveCaptureCount_, LastMoveFreedCount_,
								new Stack<DotPosition>(ChainDotsPositions_), new Stack<DotPosition>(SurroundDotsPositions_),
								new List<int>(ChainPositions_), new List<int>(SurroundPositions_))
					});

				CurrentPlayer_ = CurrentPlayer_.NextPlayer();

				LastMoveState_ = enmMoveState.Add;
				return true;
			}
			else
			{
				LastMoveState_ = enmMoveState.None;
				return false;
			}
		}

		public bool UnmakeMove()
		{
			if (DotsSequanceStates_.Count > 0)
			{
				var LastState = DotsSequanceStates_[DotsSequanceStates_.Count - 1];

				if (LastState.Base != null)
				{
					EmptyBaseCreated_ =
						LastState.Base.CaptureCountRed + LastState.Base.CaptureCountBlue == 0 ? true : false;

					foreach (var dotPosition in LastState.Base.ChainDotPositions)
						Dots_[dotPosition.Position] = dotPosition.Dot;

					foreach (var dotPosition in LastState.Base.SurrroundDotPositions)
						Dots_[dotPosition.Position] = dotPosition.Dot;

					LastMoveCaptureCount_ = LastState.Base.CaptureCountRed;
					LastMoveFreedCount_ = LastState.Base.CaptureCountBlue;
					SubCapturedFreedCount(CurrentPlayer_.NextPlayer());

					ChainDotsPositions_ = LastState.Base.ChainDotPositions;
					SurroundDotsPositions_ = LastState.Base.SurrroundDotPositions;
					ChainPositions_ = LastState.Base.ChainPositions;
					SurroundPositions_ = LastState.Base.SurroundPoistions;
				}
				else
				{
					ChainPositions_.Clear();
					SurroundPositions_.Clear();
					ChainDotsPositions_.Clear();
					SurroundDotsPositions_.Clear();
					LastMoveCaptureCount_ = 0;
					LastMoveFreedCount_ = 0;
				}

				Dots_[LastState.Move.Position] = LastState.Move.Dot;
				CurrentPlayer_ = CurrentPlayer_.NextPlayer();
				DotsSequanceStates_.RemoveAt(DotsSequanceStates_.Count - 1);

				//GetPosition(LastState.Move.Position, out LastX_, out LastY_);
				LastPosition_ = LastState.Move.Position;
				LastMoveState_ = enmMoveState.Remove;
				return true;
			}
			else
			{
				LastMoveState_ = enmMoveState.None;
				return false;
			}
		}

		public bool UnmakeAllMoves()
		{
			while (DotsSequanceStates_.Count != 0)
				if (!UnmakeMove())
					return false;
			return true;
		}

		#endregion

		#region Methods for tests

		public bool IsEmpty
		{
			get
			{
				for (int i = 0; i < Width; i++)
					for (int j = 0; j < Height; j++)
						if (Dots_[GetPosition(i, j)] != Dot.Empty)
							return false;
				return true;
			}
		}

		public IEnumerable<DotPosition> NotZeroPositions
		{
			get
			{
				var result = new List<DotPosition>();
				for (int i = 0; i < Width; i++)
					for (int j = 0; j < Height; j++)
						if (Dots_[GetPosition(i, j)] != Dot.Empty)
							result.Add(new DotPosition(GetPosition(i, j), Dots_[GetPosition(i, j)]));
				return result;
			}
		}

		#endregion
	}
}
