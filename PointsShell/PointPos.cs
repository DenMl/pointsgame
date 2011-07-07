namespace PointsShell
{
    // Описание точки в конкретной позиции.
    public struct PointPos
    {
        public GamePoint Point;
        public Pos Pos;

        public PointPos(Pos Pos, GamePoint Point)
        {
            this.Pos = Pos;
            this.Point = Point;
        }
    }
}
