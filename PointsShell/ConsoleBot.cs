using System;
using System.Collections.Generic;
using System.Diagnostics;
using PointsShell.Enums;

namespace PointsShell
{
	public class ConsoleBot : IBot
	{
		const string BotName = "PointsBot.exe";

		private Process _bot;

		private Random _random;

		private HashSet<string> _commands;

		public ConsoleBot(int width, int height, SurroundCond surCond, BeginPattern beginPattern)
		{
			Init(width, height, surCond, beginPattern);
		}

		~ConsoleBot()
		{
			Final();
		}

		public void Init(int width, int height, SurroundCond surCond, BeginPattern beginPattern)
		{
			if (_bot != null)
				Final();
			_random = new Random();
			_commands = new HashSet<string>();
			_bot = new Process
			      	{
			      		EnableRaisingEvents = false,
						PriorityClass = ProcessPriorityClass.Idle,
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
			if (!_bot.Start())
				throw new Exception("Error while starting.");
			// list_commands
			var id = _random.Next();
			_bot.StandardInput.WriteLine("{0} list_commands", id);
			var answer = _bot.StandardOutput.ReadLine();
			if (answer == null)
				throw new Exception("list_commands: Answer is null.");
			var splittedAnswer = answer.Split();
			if (splittedAnswer.Length < 4 || (splittedAnswer[0] != "=" && splittedAnswer[0] != "?") || (splittedAnswer[1] != id.ToString()) || (splittedAnswer[2] != "list_commands"))
				throw new Exception("list_commands: Invalid answer.");
			for (var i = 3; i < splittedAnswer.Length; i++)
				_commands.Add(splittedAnswer[i]);
			// init.
			if (!_commands.Contains("init"))
				throw new Exception("init: Not supported.");
			id = _random.Next();
			_bot.StandardInput.WriteLine("{0} init {1} {2} {3}", id, width, height, 78526081);
			answer = _bot.StandardOutput.ReadLine();
			if (answer == null)
				throw new Exception("init: Answer is null.");
			splittedAnswer = answer.Split();
			if (splittedAnswer.Length != 3 || (splittedAnswer[0] != "=" && splittedAnswer[0] != "?") || (splittedAnswer[1] != id.ToString()) || (splittedAnswer[2] != "init"))
				throw new Exception("init: Invalid answer.");
			if (splittedAnswer[0] == "?")
				throw new Exception("init: Error while executing.");
		}

		public void Final()
		{
			if (_bot == null)
				return;
			_bot.StandardInput.WriteLine("{0} quit", _random.Next());
			_bot.WaitForExit(50);
			if (!_bot.HasExited)
				_bot.Kill();
			_bot = null;
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
