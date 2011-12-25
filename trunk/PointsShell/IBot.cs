namespace PointsShell
{
	public interface IBot
	{
		void Init(int width, int height);
		void PutPoint(Pos pos, PlayerColor player);
		void RemoveLastPoint();
		Pos GetBotMovie(PlayerColor player);
		Pos GetBotMovieWithComplexity(PlayerColor player, int complexity);
		Pos GetBotMovieWithTime(PlayerColor player, int time);
		string GetName();
		string GetVersion();
	}
}
