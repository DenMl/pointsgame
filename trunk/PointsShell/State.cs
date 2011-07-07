using System.Collections.Generic;

namespace PointsShell
{
    public class State
    {
        // Список изменений точек на позициях.
        public List<PointPos> PointPoses = new List<PointPos>();
        // Изменение счета захваченных точек игроков.
        public int CaptureCountsRed;
        public int CaptureCountsBlack;
        public PlayerColor Player;
    }
}
