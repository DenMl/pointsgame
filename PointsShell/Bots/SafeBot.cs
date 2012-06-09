using System;
using System.Collections.Generic;
using System.Threading;
using System.Windows;
using PointsShell.Enums;

namespace PointsShell.Bots
{
	class SafeBot : IDisposable
	{
		private readonly IBot _bot;

		private bool _executing;

		private bool _error;

		private readonly Queue<Action> _actions;

		private readonly object _syncObj;

		private Thread _thread;

		public SafeBot(IBot bot)
		{
			_bot = bot;
			_actions = new Queue<Action>();
			_syncObj = new object();
		}

		private void ExecuteNext()
		{
			if (_actions.Count == 0)
				return;
			lock (_syncObj)
			{
				if (_executing)
					return;
				_executing = true;
			}
			_thread = new Thread(() =>
									{
										try
										{
											while (_actions.Count != 0)
											{
												var curAction = _actions.Dequeue();
												curAction();
											}
										}
										catch (ThreadAbortException)
										{ }
										catch (Exception e)
										{
											_error = true;
											MessageBox.Show(e.Message, "PointsShell", MessageBoxButton.OK, MessageBoxImage.Error);
										}
										_executing = false;
										ExecuteNext();
									});
			_thread.Start();
		}

		public void Init(int width, int height, SurroundCond surCond, BeginPattern beginPattern, Action initSuccess = null)
		{
			if (_error)
				return;
			_actions.Enqueue(() =>
			                 	{
			                 		_bot.Init(width, height, surCond, beginPattern);
									if (initSuccess != null)
										initSuccess();
			                 	});
			ExecuteNext();
		}

		public void Final(Action finalSuccess = null)
		{
			if (_error)
				return;
			_actions.Enqueue(() =>
			                 	{
			                 		_bot.Final();
									if (finalSuccess != null)
										finalSuccess();
			                 	});
			ExecuteNext();
		}

		public void PutPoint(Pos pos, PlayerColor player, Action putPointSuccess = null)
		{
			if (_error)
				return;
			_actions.Enqueue(() =>
			                 	{
			                 		_bot.PutPoint(pos, player);
									if (putPointSuccess != null)
										putPointSuccess();
			                 	});
			ExecuteNext();
		}

		public void RemoveLastPoint(Action removeLastPointSuccess = null)
		{
			if (_error)
				return;
			_actions.Enqueue(() =>
			                 	{
			                 		_bot.RemoveLastPoint();
									if (removeLastPointSuccess != null)
										removeLastPointSuccess();
			                 	});
			ExecuteNext();
		}

		public void GetMove(PlayerColor player, Action<Pos, TimeSpan> getMoveSuccess)
		{
			if (_error)
				return;
			_actions.Enqueue(() =>
								{
									var startTime = DateTime.Now;
			                 		var pos = _bot.GetMove(player);
			                 		if (getMoveSuccess != null)
										getMoveSuccess(pos, DateTime.Now - startTime);
								});
			ExecuteNext();
		}

		public void GetMove(PlayerColor player, Action<Pos> getMoveSuccess)
		{
			if (_error)
				return;
			_actions.Enqueue(() =>
								{
									var pos = _bot.GetMove(player);
									if (getMoveSuccess != null)
										getMoveSuccess(pos);
								});
			ExecuteNext();
		}

		public void GetMoveWithComplexity(PlayerColor player, int complexity, Action<Pos, TimeSpan> getMoveWithComplexitySuccess)
		{
			if (_error)
				return;
			_actions.Enqueue(() =>
								{
									var startTime = DateTime.Now;
			                 		var pos = _bot.GetMoveWithComplexity(player, complexity);
			                 		if (getMoveWithComplexitySuccess != null)
			                 			getMoveWithComplexitySuccess(pos, DateTime.Now - startTime);
								});
			ExecuteNext();
		}

		public void GetMoveWithComplexity(PlayerColor player, int complexity, Action<Pos> getMoveWithComplexitySuccess)
		{
			if (_error)
				return;
			_actions.Enqueue(() =>
								{
			                 		var pos = _bot.GetMoveWithComplexity(player, complexity);
									if (getMoveWithComplexitySuccess != null)
										getMoveWithComplexitySuccess(pos);
								});
			ExecuteNext();
		}

		public void GetMoveWithTime(PlayerColor player, int time, Action<Pos, TimeSpan> getMoveWithTimeSuccess)
		{
			if (_error)
				return;
			_actions.Enqueue(() =>
								{
									var startTime = DateTime.Now;
			                 		var pos = _bot.GetMoveWithTime(player, time);
			                 		if (getMoveWithTimeSuccess != null)
										getMoveWithTimeSuccess(pos, DateTime.Now - startTime);
								});
			ExecuteNext();
		}

		public void GetMoveWithTime(PlayerColor player, int time, Action<Pos> getMoveWithTimeSuccess)
		{
			if (_error)
				return;
			_actions.Enqueue(() =>
								{
									var pos = _bot.GetMoveWithTime(player, time);
									if (getMoveWithTimeSuccess != null)
										getMoveWithTimeSuccess(pos);
								});
			ExecuteNext();
		}

		public void GetName(Action<string> getNameSuccess)
		{
			if (_error)
				return;
			_actions.Enqueue(() =>
								{
									var name = _bot.GetName();
									if (getNameSuccess != null)
										getNameSuccess(name);
								});
			ExecuteNext();
		}

		public void GetVersion(Action<string> getVersionSuccess)
		{
			if (_error)
				return;
			_actions.Enqueue(() =>
								{
									var version = _bot.GetVersion();
									if (getVersionSuccess != null)
										getVersionSuccess(version);
								});
			ExecuteNext();
		}

		public void Dispose()
		{
			if (_thread != null && _thread.IsAlive)
				_thread.Abort();
			_bot.Dispose();
		}
	}
}
