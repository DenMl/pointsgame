using System.Collections.Generic;

namespace PointsShell
{
    // Описание изменения состояния поля. 
    public class StateChange
    {
        public readonly List<State> States = new List<State>();

        public void AddNewBase()
        {
            States.Add(new State());
        }

        public void AddPosPoint(Pos Pos, GamePoint Point)
        {
            States[States.Count - 1].PointPoses.Add(new PointPos(Pos, Point));
        }

        public void AddCaptureCount(int CaptureCountRed, int CaptureCountBlack)
        {
            States[States.Count - 1].CaptureCountsRed = CaptureCountRed;
            States[States.Count - 1].CaptureCountsBlack = CaptureCountBlack;
        }

        public void AddPlayer(PlayerColor Player)
        {
            States[States.Count - 1].Player = Player;
        }

        public void DeleteLastState()
        {
            States.RemoveAt(States.Count - 1);
        }
    }
}
