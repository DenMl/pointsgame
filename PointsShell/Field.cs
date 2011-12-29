using System.Collections.Generic;
using PointsShell.Enums;

namespace PointsShell
{
	public class Field
	{
		// Ширина поля.
		public int Width { get; protected set; }

		// Высота поля.
		public int Height { get; protected set; }

		public SurroundCond SurCond { get; protected set; }

		public int CaptureCountRed { get; protected set; }

		public int CaptureCountBlack { get; protected set; }

		public PlayerColor CurPlayer { get; set; }

		public PlayerColor EnemyPlayer { get { return NextPlayer(CurPlayer); } }

		protected int DCaptureCount;

		protected int DFreedCount;

		// Глобальный массив точек.
		public GamePoint[,] Points { get; protected set; }

		// Последовательность поставленных точек.
		public List<Pos> PointsSeq { get; protected set; }

		protected readonly StateChange MainState;

		// Количество поставленных точек.
		public int PointsCount
		{
			get { return PointsSeq.Count; }
		}

		public List<List<Pos>> LastChains;

		public Field(int width, int height, SurroundCond surCond)
		{
			Width = width;
			Height = height;
			SurCond = surCond;
			CaptureCountRed = 0;
			CaptureCountBlack = 0;
			CurPlayer = PlayerColor.Red;

			Points = new GamePoint[width + 2, height + 2];
			PointsSeq = new List<Pos>();
			MainState = new StateChange();

			// Помечаем граничные точки как невалидные.
			for (var i = 0; i < width + 2; i++)
			{
				Points[i, 0].Bad = true;
				Points[i, height + 1].Bad = true;
			}
			for (var i = 0; i < height + 2; i++)
			{
				Points[0, i].Bad = true;
				Points[width + 1, i].Bad = true;
			}
		}

		public static PlayerColor NextPlayer(PlayerColor player)
		{
			return player == PlayerColor.Red ? PlayerColor.Black : PlayerColor.Red;
		}

		public void SetNextPlayer()
		{
			CurPlayer = NextPlayer(CurPlayer);
		}

		public void PlaceBeginPattern(BeginPattern beginPattern)
		{
			Pos pos;
			switch (beginPattern)
			{
				case (BeginPattern.CrosswisePattern):
					pos = new Pos(Width / 2 - 1, Height / 2 - 1);
					PutPoint(pos);
					pos.X++;
					PutPoint(pos);
					pos.Y++;
					PutPoint(pos);
					pos.X--;
					PutPoint(pos);
					break;
				case (BeginPattern.SquarePattern):
					pos = new Pos(Width / 2 - 1, Height / 2 - 1);
					PutPoint(pos);
					pos.X++;
					PutPoint(pos);
					pos.Y++;
					pos.X--;
					PutPoint(pos);
					pos.X++;
					PutPoint(pos);
					break;
			}
		}

		// Направленная площадь параллелограмма с вершинами в (0; 0) и (Pos1, Pos2).
		static int Square(Pos pos1, Pos pos2)
		{
			return pos1.X * pos2.Y - pos1.Y * pos2.X;
		}

		// Возвращает количество групп точек рядом с CenterPos.
		// InpChainPoints - возможные точки цикла, InpSurPoints - возможные окруженные точки.
		int GetInputPoints(Pos centerPos, PlayerColor player, out List<Pos> inpChainPoints, out List<Pos> inpSurPoints)
		{
			inpChainPoints = new List<Pos>(4);
			inpSurPoints = new List<Pos>(4);

			if (!Points[centerPos.X - 1, centerPos.Y].Enabled(player))
			{
				if (Points[centerPos.X - 1, centerPos.Y - 1].Enabled(player))
				{
					inpChainPoints.Add(new Pos(centerPos.X - 1, centerPos.Y - 1));
					inpSurPoints.Add(new Pos(centerPos.X - 1, centerPos.Y));
				}
				else if (Points[centerPos.X, centerPos.Y - 1].Enabled(player))
				{
					inpChainPoints.Add(new Pos(centerPos.X, centerPos.Y - 1));
					inpSurPoints.Add(new Pos(centerPos.X - 1, centerPos.Y));
				}
			}

			if (!Points[centerPos.X, centerPos.Y + 1].Enabled(player))
			{
				if (Points[centerPos.X - 1, centerPos.Y + 1].Enabled(player))
				{
					inpChainPoints.Add(new Pos(centerPos.X - 1, centerPos.Y + 1));
					inpSurPoints.Add(new Pos(centerPos.X, centerPos.Y + 1));
				}
				else if (Points[centerPos.X - 1, centerPos.Y].Enabled(player))
				{
					inpChainPoints.Add(new Pos(centerPos.X - 1, centerPos.Y));
					inpSurPoints.Add(new Pos(centerPos.X, centerPos.Y + 1));
				}
			}

			if (!Points[centerPos.X + 1, centerPos.Y].Enabled(player))
			{
				if (Points[centerPos.X + 1, centerPos.Y + 1].Enabled(player))
				{
					inpChainPoints.Add(new Pos(centerPos.X + 1, centerPos.Y + 1));
					inpSurPoints.Add(new Pos(centerPos.X + 1, centerPos.Y));
				}
				else if (Points[centerPos.X, centerPos.Y + 1].Enabled(player))
				{
					inpChainPoints.Add(new Pos(centerPos.X, centerPos.Y + 1));
					inpSurPoints.Add(new Pos(centerPos.X + 1, centerPos.Y));
				}
			}

			if (!Points[centerPos.X, centerPos.Y - 1].Enabled(player))
			{
				if (Points[centerPos.X + 1, centerPos.Y - 1].Enabled(player))
				{
					inpChainPoints.Add(new Pos(centerPos.X + 1, centerPos.Y - 1));
					inpSurPoints.Add(new Pos(centerPos.X, centerPos.Y - 1));
				}
				else if (Points[centerPos.X + 1, centerPos.Y].Enabled(player))
				{
					inpChainPoints.Add(new Pos(centerPos.X + 1, centerPos.Y));
					inpSurPoints.Add(new Pos(centerPos.X, centerPos.Y - 1));
				}
			}

			return inpChainPoints.Count;
		}

		//  * . .   x . *   . x x   . . .
		//  . o .   x o .   . o .   . o x
		//  x x .   . . .   . . *   * . x
		//  o - center pos
		//  x - pos
		//  * - result
		static void GetFirstNextPos(Pos centerPos, ref Pos pos)
		{
			var dx = pos.X - centerPos.X;
			var dy = pos.Y - centerPos.Y;
			switch (dy)
			{
				case -1:
					if (dx >= 0)
					{
						pos.X = centerPos.X + 1;
						pos.Y = centerPos.Y + 1;
					}
					else
					{
						pos.X = centerPos.X + 1;
						pos.Y = centerPos.Y - 1;
					}
					break;
				case 1:
					if (dx <= 0)
					{
						pos.X = centerPos.X - 1;
						pos.Y = centerPos.Y - 1;
					}
					else
					{
						pos.X = centerPos.X - 1;
						pos.Y = centerPos.Y + 1;
					}
					break;
				default:
					if (dx == 1)
					{
						pos.X = centerPos.X - 1;
						pos.Y = centerPos.Y + 1;
					}
					else
					{
						pos.X = centerPos.X + 1;
						pos.Y = centerPos.Y - 1;
					}
					break;
			}
		}

		//  . . .   * . .   x * .   . x *   . . x   . . .   . . .   . . .
		//  * o .   x o .   . o .   . o .   . o *   . o x   . o .   . o .
		//  x . .   . . .   . . .   . . .   . . .   . . *   . * x   * x .
		//  o - center pos
		//  x - pos
		//  * - result
		static void GetNextPos(Pos centerPos, ref Pos pos)
		{
			var dx = pos.X - centerPos.X;
			var dy = pos.Y - centerPos.Y;
			switch (dy)
			{
				case -1:
					if (dx <= 0)
						pos.X++;
					else
						pos.Y++;
					break;
				case 1:
					if (dx >= 0)
						pos.X--;
					else
						pos.Y--;
					break;
				default:
					if (dx == 1)
						pos.Y++;
					else
						pos.Y--;
					break;
			}
		}

		protected void CheckCapturedAndFreed(Pos pos, PlayerColor player, ref int Captured, ref int Free)
		{
			if (!Points[pos.X, pos.Y].Putted)
				return;
			if (Points[pos.X, pos.Y].Color != player)
				Captured++;
			else
				if (Points[pos.X, pos.Y].Surrounded)
					Free++;
		}

		// Волновой алгоритм для определения захваченной зоны.
		// Начинает работать со StartPos.
		protected void CapturedAndFreedCount(Pos startPos, PlayerColor player, out int captured, out int freed, out List<Pos> surPoints)
		{
			surPoints = new List<Pos>();
			captured = 0;
			freed = 0;
			var pos = startPos;
			var tempStack = new Stack<Pos>();
			tempStack.Push(startPos);
			Points[pos.X, pos.Y].Tagged = true;

			while (tempStack.Count != 0)
			{
				pos = tempStack.Pop();
				CheckCapturedAndFreed(pos, player, ref captured, ref freed);
				surPoints.Add(new Pos(pos));

				if (!Points[pos.X - 1, pos.Y].IsBound(player) && !Points[pos.X - 1, pos.Y].Tagged)
				{
					tempStack.Push(new Pos(pos.X - 1, pos.Y));
					Points[pos.X - 1, pos.Y].Tagged = true;
				}

				if (!Points[pos.X, pos.Y - 1].IsBound(player) && !Points[pos.X, pos.Y - 1].Tagged)
				{
					tempStack.Push(new Pos(pos.X, pos.Y - 1));
					Points[pos.X, pos.Y - 1].Tagged = true;
				}

				if (!Points[pos.X + 1, pos.Y].IsBound(player) && !Points[pos.X + 1, pos.Y].Tagged)
				{
					tempStack.Push(new Pos(pos.X + 1, pos.Y));
					Points[pos.X + 1, pos.Y].Tagged = true;
				}

				if (!Points[pos.X, pos.Y + 1].IsBound(player) && !Points[pos.X, pos.Y + 1].Tagged)
				{
					tempStack.Push(new Pos(pos.X, pos.Y + 1));
					Points[pos.X, pos.Y + 1].Tagged = true;
				}
			}
		}

		// Удалить пустую базу, начиная с точки StartPos.
		// StartPos не попадет в список изменений доски.
		private void RemoveEmptyBase(Pos startPos)
		{
			var tempStack = new Stack<Pos>();
			tempStack.Push(startPos);
			Points[startPos.X, startPos.Y].EmptyBase = false;

			while (tempStack.Count != 0)
			{
				var pos = tempStack.Pop();

				if (Points[pos.X - 1, pos.Y].EmptyBase)
				{
					tempStack.Push(new Pos(pos.X - 1, pos.Y));
					MainState.AddPosPoint(pos, Points[pos.X - 1, pos.Y]);
					Points[pos.X - 1, pos.Y].EmptyBase = false;
				}

				if (Points[pos.X, pos.Y - 1].EmptyBase)
				{
					tempStack.Push(new Pos(pos.X, pos.Y - 1));
					MainState.AddPosPoint(pos, Points[pos.X, pos.Y - 1]);
					Points[pos.X, pos.Y - 1].EmptyBase = false;
				}

				if (Points[pos.X + 1, pos.Y].EmptyBase)
				{
					tempStack.Push(new Pos(pos.X + 1, pos.Y));
					MainState.AddPosPoint(pos, Points[pos.X + 1, pos.Y]);
					Points[pos.X + 1, pos.Y].EmptyBase = false;
				}

				if (Points[pos.X, pos.Y + 1].EmptyBase)
				{
					tempStack.Push(new Pos(pos.X, pos.Y + 1));
					MainState.AddPosPoint(pos, Points[pos.X, pos.Y + 1]);
					Points[pos.X, pos.Y + 1].EmptyBase = false;
				}
			}
		}

		// Добавляет к счету игроков результат хода.
		protected void AddSubCapturedFreed(PlayerColor player, int captured, int freed)
		{
			if (captured == -1)
			{
				if (player == 0)
					CaptureCountRed++;
				else
					CaptureCountBlack++;
			}
			else
			{
				if (player == 0)
				{
					CaptureCountRed += captured;
					CaptureCountBlack -= freed;
				}
				else
				{
					CaptureCountBlack += captured;
					CaptureCountRed -= freed;
				}
			}
		}

		protected void SetCaptureFreeState(Pos pos, PlayerColor player)
		{
			if (Points[pos.X, pos.Y].Putted)
				Points[pos.X, pos.Y].Surrounded = Points[pos.X, pos.Y].Color != player;
			else
				Points[pos.X, pos.Y].Surrounded = true;
		}

		private bool BuildChain(Pos startPos, Pos inpChainPoint, out List<Pos> chain)
		{
			var player = Points[startPos.X, startPos.Y].Color;

			chain = new List<Pos> { startPos };
			var pos = inpChainPoint;
			var centerPos = startPos;
			// Площадь базы.
			var tempSquare = Square(centerPos, pos);
			do
			{
				if (Points[pos.X, pos.Y].Tagged)
				{
					while (chain[chain.Count - 1] != pos)
					{
						Points[chain[chain.Count - 1].X, chain[chain.Count - 1].Y].Tagged = false;
						chain.RemoveAt(chain.Count - 1);
					}
				}
				else
				{
					Points[pos.X, pos.Y].Tagged = true;
					chain.Add(pos);
				}
				pos.Swap(ref centerPos);
				GetFirstNextPos(centerPos, ref pos);
				while (!Points[pos.X, pos.Y].Enabled(player))
					GetNextPos(centerPos, ref pos);
				tempSquare += Square(centerPos, pos);
			}
			while (pos != startPos);

			for (var j = 0; j < chain.Count; j++)
				Points[chain[j].X, chain[j].Y].Tagged = false;

			return (tempSquare < 0) && (chain.Count > 2);
		}

		private int BuildChains(Pos StartPos, IList<Pos> InpChainPoints, IList<Pos> InpSurPoints, int InpPointsCount, out List<Pos>[] Chains, out Pos[] InsidePoints)
		{
			Chains = new List<Pos>[4];
			InsidePoints = new Pos[4];

			var count = 0;
			for (var i = 0; i < InpPointsCount; i++)
				if (BuildChain(StartPos, InpChainPoints[i], out Chains[count]))
				{
					InsidePoints[count] = InpSurPoints[i];
					count++;
				}

			return count;
		}

		private int BuildChainsFast(Pos startPos, IList<Pos> inpChainPoints, IList<Pos> inpSurPoints, int inpPointsCount, out List<Pos>[] chains, out Pos[] insidePoints)
		{
			chains = new List<Pos>[4];
			insidePoints = new Pos[4];

			var count = 0;
			for (var i = 0; i < inpPointsCount; i++)
				if (BuildChain(startPos, inpChainPoints[i], out chains[count]))
				{
					insidePoints[count] = inpSurPoints[i];
					count++;

					if (count == inpPointsCount - 1)
						break;
				}

			return count;
		}

		protected virtual void FindSurround(List<Pos> chain, Pos insidePoint, PlayerColor player)
		{
			int curCaptureCount, curFreedCount;
			List<Pos> surPoints;

			// Помечаем точки цепочки.
			for (var i = 0; i < chain.Count; i++)
				Points[chain[i].X, chain[i].Y].Tagged = true;

			// Заливка захваченной области.
			CapturedAndFreedCount(insidePoint, player, out curCaptureCount, out curFreedCount, out surPoints);
			// Изменение счета игроков.
			AddSubCapturedFreed(player, curCaptureCount, curFreedCount);
			DCaptureCount += curCaptureCount;
			DFreedCount += curFreedCount;

			if ((curCaptureCount != 0) || (SurCond == SurroundCond.Always)) // Если захватили точки, или стоит опция захватывать всегда.
			{
				for (var i = 0; i < chain.Count; i++)
				{
					Points[chain[i].X, chain[i].Y].Tagged = false;
					// Добавляем в список изменений точки цепочки.
					MainState.AddPosPoint(chain[i], Points[chain[i].X, chain[i].Y]);
					// Помечаем точки цепочки.
					Points[chain[i].X, chain[i].Y].Bound = true;
				}

				for (var j = 0; j < surPoints.Count; j++)
				{
					Points[surPoints[j].X, surPoints[j].Y].Tagged = false;

					MainState.AddPosPoint(surPoints[j], Points[surPoints[j].X, surPoints[j].Y]);

					SetCaptureFreeState(surPoints[j], player);
				}

				LastChains.Add(chain);
			}
			else // Если ничего не захватили.
			{
				for (var i = 0; i < chain.Count; i++)
					Points[chain[i].X, chain[i].Y].Tagged = false;

				for (var i = 0; i < surPoints.Count; i++)
				{
					Points[surPoints[i].X, surPoints[i].Y].Tagged = false;

					MainState.AddPosPoint(surPoints[i], Points[surPoints[i].X, surPoints[i].Y]);

					if (!Points[surPoints[i].X, surPoints[i].Y].Putted)
						Points[surPoints[i].X, surPoints[i].Y].EmptyBase = true;
				}
			}
		}

		protected void FindSurrounds(List<Pos>[] chains, Pos[] insidePoints, int chainsCount, PlayerColor player)
		{
			for (var i = 0; i < chainsCount; i++)
				FindSurround(chains[i], insidePoints[i], player);
		}

		private enum IntersectionState
		{
			None,
			Up,
			Down,
			Target
		}
		private static IntersectionState GetIntersectionState(Pos pos, Pos nextPos)
		{
			if (nextPos.X <= pos.X)
				switch (nextPos.Y - pos.Y)
				{
					case (1):
						return IntersectionState.Up;
					case (0):
						return IntersectionState.Target;
					case (-1):
						return IntersectionState.Down;
					default:
						return IntersectionState.None;
				}
			return IntersectionState.None;
		}

		protected bool PointInsideRing(Pos testedPos, List<Pos> ring)
		{
			var intersections = 0;

			var state = IntersectionState.None;

			foreach (var pos in ring)
			{
				switch (GetIntersectionState(testedPos, pos))
				{
					case (IntersectionState.None):
						state = IntersectionState.None;
						break;
					case (IntersectionState.Up):
						if (state == IntersectionState.Down)
							intersections++;
						state = IntersectionState.Up;
						break;
					case (IntersectionState.Down):
						if (state == IntersectionState.Up)
							intersections++;
						state = IntersectionState.Down;
						break;
				}
			}
			if (state == IntersectionState.Up || state == IntersectionState.Down)
			{
				var tempState = GetIntersectionState(testedPos, ring[0]);
				var i = 0;
				while (tempState == state || tempState == IntersectionState.Target)
				{
					i++;
					tempState = GetIntersectionState(testedPos, ring[i]);
				}
				if (tempState != IntersectionState.None)
					intersections++;
			}

			return intersections % 2 == 1;
		}

		protected void CheckClosure(Pos startPos)
		{
			int inpPointsCount;
			List<Pos> inpChainPoints, inpSurPoints;

			List<Pos>[] chains;
			Pos[] insidePoints;
			int chainsCount;

			DCaptureCount = 0;
			DFreedCount = 0;

			LastChains = new List<List<Pos>>();

			// Цвет игрока, сделавшего последний ход.
			var outPlayer = Points[startPos.X, startPos.Y].Color;

			if (Points[startPos.X, startPos.Y].EmptyBase) // Если точка поставлена в пустую базу.
			{
				// Проверяем, в чью пустую базу поставлена точка.
				var pos = new Pos(startPos.X - 1, startPos.Y);
				while (!Points[pos.X, pos.Y].Putted)
					pos.X--;

				if (Points[pos.X, pos.Y].Color == Points[startPos.X, startPos.Y].Color) // Если поставили в свою пустую базу.
				{
					Points[startPos.X, startPos.Y].EmptyBase = false;
					return;
				}

				if (SurCond != SurroundCond.AlwaysEnemy) // Если приоритет не всегда у врага.
				{
					inpPointsCount = GetInputPoints(startPos, outPlayer, out inpChainPoints, out inpSurPoints);
					if (inpPointsCount > 1)
					{
						chainsCount = BuildChainsFast(startPos, inpChainPoints, inpSurPoints, inpPointsCount, out chains, out insidePoints);
						FindSurrounds(chains, insidePoints, chainsCount, outPlayer);
						if (Points[startPos.X, startPos.Y].Bound)
						{
							RemoveEmptyBase(startPos);
							return;
						}
					}
				}

				pos.X++;
				do
				{
					do
					{
						pos.X--;
						while (!Points[pos.X, pos.Y].Enabled(NextPlayer(outPlayer)))
							pos.X--;
						inpPointsCount = GetInputPoints(pos, Points[pos.X, pos.Y].Color, out inpChainPoints, out inpSurPoints);
						chainsCount = BuildChains(pos, inpChainPoints, inpSurPoints, inpPointsCount, out chains, out insidePoints);
					} while (chainsCount == 0);

					for (var i = 0; i < chainsCount; i++)
						if (PointInsideRing(startPos, chains[i]))
						{
							FindSurround(chains[i], insidePoints[i], Points[pos.X, pos.Y].Color);
							break;
						}
				} while (!Points[startPos.X, startPos.Y].Surrounded);

				DCaptureCount = -1;
				return;
			}

			inpPointsCount = GetInputPoints(startPos, outPlayer, out inpChainPoints, out inpSurPoints);
			if (inpPointsCount > 1)
			{
				chainsCount = BuildChainsFast(startPos, inpChainPoints, inpSurPoints, inpPointsCount, out chains, out insidePoints);
				FindSurrounds(chains, insidePoints, chainsCount, outPlayer);
			}
		}

		public virtual void BackMove()
		{
			CaptureCountRed = MainState.States[MainState.States.Count - 1].CaptureCountsRed;
			CaptureCountBlack = MainState.States[MainState.States.Count - 1].CaptureCountsBlack;
			for (var i = MainState.States[MainState.States.Count - 1].PointPoses.Count - 1; i >= 0; i--) // Делаем отмену обязательно в обратном порядке, так как возможны дублирования позции (в частности, когда происходит окружение, точка, приведшая к окружению будет в изменениях 2 раза).
				// Стоит подумать над откатом поставленной точки отдельно. Но проблема - посталенная точка в свою пустую базу.
				Points[MainState.States[MainState.States.Count - 1].PointPoses[i].Pos.X, MainState.States[MainState.States.Count - 1].PointPoses[i].Pos.Y] = MainState.States[MainState.States.Count - 1].PointPoses[i].Point;
			CurPlayer = MainState.States[MainState.States.Count - 1].Player;
			MainState.DeleteLastState();
			PointsSeq.RemoveAt(PointsCount - 1);
		}

		public bool PutPoint(Pos point)
		{
			if (PutPoint(point, CurPlayer))
			{
				SetNextPlayer();
				return true;
			}
			return false;
		}

		public virtual bool PutPoint(Pos point, PlayerColor player)
		{
			if (point.X < 1 || point.X > Width || point.Y < 1 || point.Y > Height)
				return false;

			if (!Points[point.X, point.Y].PuttingAllow())
				return false;

			// Добавляем новый список изменений поля.
			MainState.AddNewBase();
			// Запоминаем захваченные точки и поля игроков.
			MainState.AddCaptureCount(CaptureCountRed, CaptureCountBlack);
			// Запоминаем игрока, чей ход сейчас должен быть.
			MainState.AddPlayer(CurPlayer);

			MainState.AddPosPoint(point, Points[point.X, point.Y]);

			Points[point.X, point.Y].Putted = true;
			Points[point.X, point.Y].Color = player;
			PointsSeq.Add(point);

			CheckClosure(point);

			return true;
		}
	}
}