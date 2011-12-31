using System;
using System.Diagnostics;
using PointsShell.Enums;

namespace PointsShell
{
	public class ConsoleBot : IBot
	{
		const string BotName = "PointsBot.exe";

		protected Process Bot;

		public void Init(int width, int height, SurroundCond surCond, BeginPattern beginPattern)
		{
			Bot = new Process
			      	{
			      		EnableRaisingEvents = false,
			      		StartInfo =
			      			{
			      				UseShellExecute = false,
								RedirectStandardInput = true,
			      				RedirectStandardOutput = true,
			      				RedirectStandardError = true,
			      				CreateNoWindow = true,
			      				FileName = BotName
			      			}
			      	};
			Bot.Start();
			Bot.StandardInput.WriteLine("{0} init {1} {2} {3}", 0, width, height, 78526081);
			var answer = Bot.StandardOutput.ReadLine();
			if (answer == null)
				throw new Exception();
			if (answer[0] != '=')
				throw new Exception(answer);
		}

		public void PutPoint(Pos pos, PlayerColor player)
		{
			throw new NotImplementedException();
		}

		public void RemoveLastPoint()
		{
			throw new NotImplementedException();
		}

		public Pos GetMove(PlayerColor player)
		{
			throw new NotImplementedException();
		}

		public Pos GetMoveWithComplexity(PlayerColor player, int complexity)
		{
			throw new NotImplementedException();
		}

		public Pos GetMoveWithTime(PlayerColor player, int time)
		{
			throw new NotImplementedException();
		}

		public string GetName()
		{
			throw new NotImplementedException();
		}

		public string GetVersion()
		{
			throw new NotImplementedException();
		}
	}
}
