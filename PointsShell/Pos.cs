namespace PointsShell
{
	// Описание позиции.
	public struct Pos
	{
		public int X;
		public int Y;

		public Pos(int X, int Y)
		{
			this.X = X;
			this.Y = Y;
		}

		public Pos(Pos Pos)
		{
			X = Pos.X;
			Y = Pos.Y;
		}

		public static bool operator ==(Pos Pos1, Pos Pos2)
		{
			return Pos1.X == Pos2.X && Pos1.Y == Pos2.Y;
		}

		public static bool operator !=(Pos Pos1, Pos Pos2)
		{
			return Pos1.X != Pos2.X || Pos1.Y != Pos2.Y;
		}

		public void Swap(ref Pos Pos)
		{
			var TempPos = new Pos(Pos.X, Pos.Y);
			Pos.X = X;
			Pos.Y = Y;
			X = TempPos.X;
			Y = TempPos.Y;
		}
	}
}
