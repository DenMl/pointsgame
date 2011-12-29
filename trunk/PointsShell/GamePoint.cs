using PointsShell.Enums;

namespace PointsShell
{
	// Описание точки.
	public struct GamePoint
	{
		// Цвет точки.
		public PlayerColor Color { get; set; }
		// Поставлена ли точка.
		public bool Putted { get; set; }
		// Окружена ли точка.
		public bool Surrounded { get; set; }
		// Окружает ли что-то точка.
		public bool Bound { get; set; }
		// Находится ли в пустой базе точка.
		public bool EmptyBase { get; set; }
		// Вспомогательный бит (используется при работе алгоритма "заливки" точек в базе).
		public bool Tagged { get; set; }
		// Указывает на то, что координаты точки невалидны.
		public bool Bad { get; set; }

		public GamePoint(GamePoint point) : this()
		{
			Color = point.Color;
			Putted = point.Putted;
			Surrounded = point.Surrounded;
			Bound = point.Bound;
			EmptyBase = point.EmptyBase;
			Tagged = point.Tagged;
			Bad = point.Bad;
		}

		// Проверка наличия точки игрока.
		public bool Enabled(PlayerColor color)
		{
			return Color == color && Putted && !Surrounded;
		}

		// Окружает ли что-то точка.
		public bool IsBound(PlayerColor color)
		{
			return Color == color && Putted && !Surrounded && Bound;
		}

		// Возможно ли поставить точку.
		public bool PuttingAllow()
		{
			return !Putted && !Surrounded && !Bad;
		}
	}
}
