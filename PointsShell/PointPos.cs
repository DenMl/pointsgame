namespace PointsShell
{
    // Описание точки в конкретной позиции.
    public struct PointPos
    {
        public GamePoint Point;
        public Pos Pos;

        public PointPos(Pos pos, GamePoint point)
        {
            Pos = pos;
            Point = point;
        }
    }
}
