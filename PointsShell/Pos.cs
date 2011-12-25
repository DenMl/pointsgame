namespace PointsShell
{
	// Описание позиции.
	public struct Pos
	{
		public int X;
		public int Y;

		public Pos(int x, int y)
		{
			X = x;
			Y = y;
		}

		public Pos(Pos pos)
		{
			X = pos.X;
			Y = pos.Y;
		}

		public static bool operator ==(Pos pos1, Pos pos2)
		{
			return pos1.X == pos2.X && pos1.Y == pos2.Y;
		}

		public static bool operator !=(Pos pos1, Pos pos2)
		{
			return pos1.X != pos2.X || pos1.Y != pos2.Y;
		}

		public void Swap(ref Pos pos)
		{
			var tempX = pos.X;
			var tempY = pos.Y;
			pos.X = X;
			pos.Y = Y;
			X = tempX;
			Y = tempY;
		}
	}
}
