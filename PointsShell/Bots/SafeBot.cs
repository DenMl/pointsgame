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
			(new Thread(() =>
							{
								try
								{
									_bot.Init(width, height, surCond, beginPattern);
									if (InitSuccess != null)
										InitSuccess();
								}
								catch (Exception e)
								{
									if (InitError != null)
										InitError(e.Message);
								}
							})).Start();
		}

		public void Final()
		{
			(new Thread(() =>
							{
								try
								{
									_bot.Final();
									if (FinalSuccess != null)
										FinalSuccess();
								}
								catch (Exception e)
								{
									if (FinalError != null)
										FinalError(e.Message);
								}
							})).Start();
		}

		public void PutPoint(Pos pos, PlayerColor player)
		{
			(new Thread(() =>
							{
								try
								{
									_bot.PutPoint(pos, player);
									if (PutPointSuccess != null)
										PutPointSuccess();
								}
								catch (Exception e)
								{
									if (PutPointError != null)
										PutPointError(e.Message);
								}
							})).Start();
		}

		public void RemoveLastPoint()
		{
			(new Thread(() =>
							{
								try
								{
									_bot.RemoveLastPoint();
									if (RemoveLastPointSuccess != null)
										RemoveLastPointSuccess();
								}
								catch (Exception e)
								{
									if (RemoveLastPointError != null)
										RemoveLastPointError(e.Message);
								}
							})).Start();
		}

		public void GetMove(PlayerColor player)
		{
			(new Thread(() =>
							{
								try
								{
									var pos = _bot.GetMove(player);
									if (GetMoveSuccess != null)
										GetMoveSuccess(pos);
								}
								catch (Exception e)
								{
									if (GetMoveError != null)
										GetMoveError(e.Message);
								}
							})).Start();
		}	

		public void GetMoveWithComplexity(PlayerColor player, int complexity)
		{
			(new Thread(() =>
							{
								try
								{
									var pos = _bot.GetMoveWithComplexity(player, complexity);
									if (GetMoveWithComplexitySuccess != null)
										GetMoveWithComplexitySuccess(pos);
								}
								catch (Exception e)
								{
									if (GetMoveWithComplexityError != null)
										GetMoveWithComplexityError(e.Message);
								}
							})).Start();
		}

		public void GetMoveWithTime(PlayerColor player, int time)
		{
			(new Thread(() =>
							{
								try
								{
									var pos = _bot.GetMoveWithTime(player, time);
									if (GetMoveWithTimeSuccess != null)
										GetMoveWithTimeSuccess(pos);
								}
								catch (Exception e)
								{
									if (GetMoveWithTimeError != null)
										GetMoveWithTimeError(e.Message);
								}
							})).Start();
		}

		public void GetName()
		{
			(new Thread(() =>
							{
								try
								{
									var name = _bot.GetName();
									if (GetNameSuccess != null)
										GetNameSuccess(name);
								}
								catch (Exception e)
								{
									if (GetNameError != null)
										GetNameError(e.Message);
								}
							})).Start();
		}

		public void GetVersion()
		{
			(new Thread(() =>
							{
								try
								{
									var version = _bot.GetVersion();
									if (GetVersionSuccess != null)
										GetVersionSuccess(version);
								}
								catch (Exception e)
								{
									if (GetVersionError != null)
										GetVersionError(e.Message);
								}
							})).Start();
		}
	}
}
