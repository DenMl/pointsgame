namespace PointsShell
{
    // Описание точки.
    public struct GamePoint
    {
        // Цвет точки.
        public PlayerColor Color;
        // Поставлена ли точка.
        public bool Putted;
        // Окружена ли точка.
        public bool Surrounded;
        // Окружает ли что-то точка.
        public bool Bound;
        // Находится ли в пустой базе точка.
        public bool EmptyBase;
        // Вспомогательный бит (используется при работе алгоритма "заливки" точек в базе).
        public bool Tagged;

        public GamePoint(GamePoint Point)
        {
            Color = Point.Color;
            Putted = Point.Putted;
            Surrounded = Point.Surrounded;
            Bound = Point.Bound;
            EmptyBase = Point.EmptyBase;
            Tagged = Point.Tagged;
        }

        public GamePoint(PlayerColor color, bool putted, bool surrounded, bool bound, bool emptyBase, bool tagged)
        {
            Color = color;
            Putted = putted;
            Surrounded = surrounded;
            Bound = bound;
            EmptyBase = emptyBase;
            Tagged = tagged;
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
            return !Putted && !Surrounded;
        }
    }
}
