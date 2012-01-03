using System;
using System.Threading;
using PointsShell.Enums;

namespace PointsShell.Bots
{
	class SafeBot
	{
		private readonly IBot _bot;

		public event Action InitSuccess;
		public event Action FinalSuccess;
		public event Action PutPointSuccess;
		public event Action RemoveLastPointSuccess;
		public event Action<Pos> GetMoveSuccess;
		public event Action<Pos> GetMoveWithComplexitySuccess;
		public event Action<Pos> GetMoveWithTimeSuccess;
		public event Action<string> GetNameSuccess;
		public event Action<string> GetVersionSuccess;
		public event Action<string> InitError;
		public event Action<string> FinalError;
		public event Action<string> PutPointError;
		public event Action<string> RemoveLastPointError;
		public event Action<string> GetMoveError;
		public event Action<string> GetMoveWithComplexityError;
		public event Action<string> GetMoveWithTimeError;
		public event Action<string> GetNameError;
		public event Action<string> GetVersionError;

		public SafeBot(IBot bot)
		{
			_bot = bot;
		}

		public void Init(int width, int height, SurroundCond surCond, BeginPattern beginPattern)
		{	
			Init(width, height, surCond, beginPattern, InitSuccess, InitError);
		}

		public void Init(int width, int height, SurroundCond surCond, BeginPattern beginPattern, Action initSuccess, Action<string> initError = null)
		{
			(new Thread(() =>
			{
				try
				{
					_bot.Init(width, height, surCond, beginPattern);
					if (initSuccess != null)
						initSuccess();
				}
				catch (Exception e)
				{
					if (initError != null)
						initError(e.Message);
				}
			})).Start();
		}

		public void Final()
		{
			Final(FinalSuccess, FinalError);
		}

		public void Final(Action finalSuccess, Action<string> finalError = null)
		{
			(new Thread(() =>
			{
				try
				{
					_bot.Final();
					if (finalSuccess != null)
						finalSuccess();
				}
				catch (Exception e)
				{
					if (finalError != null)
						finalError(e.Message);
				}
			})).Start();
		}

		public void PutPoint(Pos pos, PlayerColor player)
		{
			PutPoint(pos, player, PutPointSuccess, PutPointError);
		}

		public void PutPoint(Pos pos, PlayerColor player, Action putPointSuccess, Action<string> putPointError = null)
		{
			(new Thread(() =>
			{
				try
				{
					_bot.PutPoint(pos, player);
					if (putPointSuccess != null)
						putPointSuccess();
				}
				catch (Exception e)
				{
					if (putPointError != null)
						putPointError(e.Message);
				}
			})).Start();
		}

		public void RemoveLastPoint()
		{
			RemoveLastPoint(RemoveLastPointSuccess, RemoveLastPointError);
		}

		public void RemoveLastPoint(Action removeLastPointSuccess, Action<string> removeLastPointError = null)
		{
			(new Thread(() =>
			{
				try
				{
					_bot.RemoveLastPoint();
					if (removeLastPointSuccess != null)
						removeLastPointSuccess();
				}
				catch (Exception e)
				{
					if (removeLastPointError != null)
						removeLastPointError(e.Message);
				}
			})).Start();
		}

		public void GetMove(PlayerColor player)
		{
			GetMove(player, GetMoveSuccess, GetMoveError);
		}

		public void GetMove(PlayerColor player, Action<Pos> getMoveSuccess, Action<string> getMoveError = null)
		{
			(new Thread(() =>
			{
				try
				{
					var pos = _bot.GetMove(player);
					if (getMoveSuccess != null)
						getMoveSuccess(pos);
				}
				catch (Exception e)
				{
					if (getMoveError != null)
						getMoveError(e.Message);
				}
			})).Start();
		}

		public void GetMoveWithComplexity(PlayerColor player, int complexity)
		{
			GetMoveWithComplexity(player, complexity, GetMoveWithComplexitySuccess, GetMoveWithComplexityError);
		}

		public void GetMoveWithComplexity(PlayerColor player, int complexity, Action<Pos> getMoveWithComplexitySuccess, Action<string> getMoveWithComplexityError = null)
		{
			(new Thread(() =>
			{
				try
				{
					var pos = _bot.GetMoveWithComplexity(player, complexity);
					if (getMoveWithComplexitySuccess != null)
						getMoveWithComplexitySuccess(pos);
				}
				catch (Exception e)
				{
					if (getMoveWithComplexityError != null)
						getMoveWithComplexityError(e.Message);
				}
			})).Start();
		}

		public void GetMoveWithTime(PlayerColor player, int time)
		{
			GetMoveWithTime(player, time, GetMoveWithTimeSuccess, GetMoveWithTimeError);
		}

		public void GetMoveWithTime(PlayerColor player, int time, Action<Pos> getMoveWithTimeSuccess, Action<string> getMoveWithTimeError = null)
		{
			(new Thread(() =>
			{
				try
				{
					var pos = _bot.GetMoveWithTime(player, time);
					if (getMoveWithTimeSuccess != null)
						getMoveWithTimeSuccess(pos);
				}
				catch (Exception e)
				{
					if (getMoveWithTimeError != null)
						getMoveWithTimeError(e.Message);
				}
			})).Start();
		}

		public void GetName()
		{
			GetName(GetNameSuccess, GetNameError);
		}

		public void GetName(Action<string> getNameSuccess, Action<string> getNameError = null)
		{
			(new Thread(() =>
			{
				try
				{
					var name = _bot.GetName();
					if (getNameSuccess != null)
						getNameSuccess(name);
				}
				catch (Exception e)
				{
					if (getNameError != null)
						getNameError(e.Message);
				}
			})).Start();
		}

		public void GetVersion()
		{
			GetVersion(GetVersionSuccess, GetVersionError);
		}

		public void GetVersion(Action<string> getVersionSuccess, Action<string> getVersionError = null)
		{
			(new Thread(() =>
			{
				try
				{
					var version = _bot.GetVersion();
					if (getVersionSuccess != null)
						getVersionSuccess(version);
				}
				catch (Exception e)
				{
					if (getVersionError != null)
						getVersionError(e.Message);
				}
			})).Start();
		}
	}
}
