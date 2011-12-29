namespace PointsShell
{
	// Описание позиции.
	public struct Pos
	{
		public int X { get; set; }
		public int Y { get; set; }

		public Pos(int x, int y) : this()
		{
			X = x;
			Y = y;
		}

		public Pos(Pos pos) : this()
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
			var temp = pos;
			pos = this;
			this = temp;
		}

		public bool Equals(Pos pos)
		{
			return pos.X == X && pos.Y == Y;
		}

		public override bool Equals(object obj)
		{
			if (ReferenceEquals(null, obj))
				return false;
			return obj is Pos && Equals((Pos)obj);
		}

		public override int GetHashCode()
		{
			return (X * 397) ^ Y;
		}
	}
}
