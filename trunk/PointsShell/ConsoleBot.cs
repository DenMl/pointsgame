using PointsShell.Enums;

namespace PointsShell
{
	public class ConsoleBot : IBot
	{
		public void Init(int width, int height, SurroundCond surCond, BeginPattern beginPattern)
		{
			throw new System.NotImplementedException();
		}

		public void PutPoint(Pos pos, PlayerColor player)
		{
			throw new System.NotImplementedException();
		}

		public void RemoveLastPoint()
		{
			throw new System.NotImplementedException();
		}

		public Pos GetMove(PlayerColor player)
		{
			throw new System.NotImplementedException();
		}

		public Pos GetMoveWithComplexity(PlayerColor player, int complexity)
		{
			throw new System.NotImplementedException();
		}

		public Pos GetMoveWithTime(PlayerColor player, int time)
		{
			throw new System.NotImplementedException();
		}

		public string GetName()
		{
			throw new System.NotImplementedException();
		}

		public string GetVersion()
		{
			throw new System.NotImplementedException();
		}
	}
}
