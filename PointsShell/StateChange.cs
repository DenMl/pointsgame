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

		public void AddPosPoint(Pos pos, GamePoint point)
		{
			States[States.Count - 1].PointPoses.Add(new PointPos(pos, point));
		}

		public void AddCaptureCount(int captureCountRed, int captureCountBlack)
		{
			States[States.Count - 1].CaptureCountsRed = captureCountRed;
			States[States.Count - 1].CaptureCountsBlack = captureCountBlack;
		}

		public void AddPlayer(PlayerColor player)
		{
			States[States.Count - 1].Player = player;
		}

		public void DeleteLastState()
		{
			States.RemoveAt(States.Count - 1);
		}
	}
}
