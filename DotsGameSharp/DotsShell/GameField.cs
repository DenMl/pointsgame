using System;
using System.Collections.Generic;
using System.Linq;
using Dots.Library;
using Dots.AI;

namespace DotsShell
{
	public partial class GameField
	{
		#region Properties

		public Field Field;

		public GameBot Bot;

		public string PlayerNameRed
		{
			get;
			protected set;
		}

		public string PlayerNameBlack
		{
			get;
			protected set;
		}

		public enmBeginPattern BeginPattern;

		MovesTree MovesTree;

		public List<int> PointSeq2;

		#endregion

		public MoveEventHandler Move;

		protected void OnMove(MoveEventArgs e)
		{
			if (Move != null)
				Move(this, e);
		}

		void PlaceBeginPattern()
		{
			int centerX, centerY;
			switch (BeginPattern)
			{
				case (enmBeginPattern.Crosswise):
					centerX = Field.Width / 2 - 1;
					centerY = Field.Height / 2 - 1;
					MakeMove(centerX++, centerY);
					MakeMove(centerX, centerY++);
					MakeMove(centerX--, centerY);
					MakeMove(centerX, centerY);
					break;
				case (enmBeginPattern.Square):
					centerX = Field.Width / 2 - 1;
					centerY = Field.Height / 2 - 1;
					MakeMove(centerX++, centerY);
					MakeMove(centerX--, centerY++);
					MakeMove(centerX++, centerY);
					MakeMove(centerX, centerY);
					break;
			}
		}		

		public GameField(string FileName)
		{

		}

		public bool MakeMove(int X, int Y)
		{
			if (X < 0 || X >= Field.Width || Y < 0 || Y >= Field.Height)
				return false;
			if (Field.MakeMove(X, Y))
			{
				MovesTree.Add(X, Y);
				OnMove(new MoveEventArgs(enmMoveState.Add, Field.CurrentPlayer.NextPlayer(), 
					Field.LastPosition,
					Field.ChainPositions, Field.SurroundPositions));

				if (Bot != null)
				{
					var botMove = Bot.MakeMove();
					Field.MakeMove(botMove);

					//MovesTree.Add();
					OnMove(new MoveEventArgs(enmMoveState.Add, Field.CurrentPlayer.NextPlayer(),
						Field.LastPosition,
						Field.ChainPositions, Field.SurroundPositions));
				}
				
				return true;
			}
			else
				return false;
		}

		public bool UnmakeMove()
		{
			if (Field.UnmakeMove())
			{
				OnMove(new MoveEventArgs(enmMoveState.Remove, Field.CurrentPlayer.NextPlayer(),
					Field.LastPosition,
					Field.ChainPositions, Field.SurroundPositions));
				return true;
			}
			else
				return false;
		}

		public void SetMoveNumber(int MoveNumber)
		{
			if (MoveNumber < Field.DotsSequanceStates.Count())
				RollbackMoves(Field.DotsSequanceStates.Count() - MoveNumber);
			else
				if (MoveNumber > Field.DotsSequanceStates.Count())
					NextMoves(MoveNumber - Field.DotsSequanceStates.Count());
		}

		public void RollbackMoves(int Count)
		{
			for (int i = 0; i < Count; i++)
				UnmakeMove();
		}

		public void NextMoves(int Count)
		{
			for (int i = 0; i < Count; i++)
				MakeMove(PointSeq2[Field.DotsSequanceStates.Count()] % Field.RealWidth - 1,
					PointSeq2[Field.DotsSequanceStates.Count()] / Field.RealWidth - 1);
		}

		#region Constructors

		public GameField()
		{
			MovesTree = new MovesTree();
			this.BeginPattern = enmBeginPattern.Crosswise;
			PlaceBeginPattern();
		}

		public GameField(Field field, enmBeginPattern beginPattern, GameBot gameBot)
		{
			Field = field;
			//Bot = gameBot;
			MovesTree = new MovesTree();
			BeginPattern = beginPattern;
			//PlaceBeginPattern();
		}

		#endregion
	}
}
