using System;
using System.Runtime.InteropServices;

namespace PointsShell
{
	class PointsBot
	{
#if DEBUG
		const string DllName = "../../../../PointsBot/Debug/PointsBot.dll";
#else
		const string DllName = "PointsBot.dll";
#endif

		private readonly IntPtr _handle;

		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
		private static extern IntPtr InitField(int width, int height, SurroundCond surCond, BeginPattern beginPattern);
		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
		private static extern void FinalField(IntPtr field);
		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
		private static extern void PutPoint(IntPtr field, int x, int y);
		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
		private static extern void PutPlayersPoint(IntPtr field, int x, int y, PlayerColor player);
		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
		private static extern void RemoveLastPoint(IntPtr field);
		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
		private static extern void GetBotMove(IntPtr field, int minMaxDepth, int UCTIterations, ref int x, ref int y);
		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
		private static extern void SetCurrentPlayer(IntPtr field, PlayerColor player);
		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
		private static extern PlayerColor GetCurrentPlayer(IntPtr field);
		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
		private static extern void SetNextPlayer(IntPtr field);

		public PointsBot(int width, int height, SurroundCond surCond, BeginPattern beginPattern)
		{
			_handle = InitField(width, height, surCond, beginPattern);
		}

		~PointsBot()
		{
			FinalField(_handle);
		}

		public void PutPoint(Pos pos)
		{
			PutPoint(_handle, pos.X - 1, pos.Y - 1);
		}

		public void PutPoint(Pos pos, PlayerColor player)
		{
			PutPlayersPoint(_handle, pos.X - 1, pos.Y - 1, player);
		}

		public void RemoveLastPoint()
		{
			RemoveLastPoint(_handle);
		}

		public Pos GetBotMovie(int minMaxDepth, int UCTIterations)
		{
			int x = 0, y = 0;
			GetBotMove(_handle, minMaxDepth, UCTIterations, ref x, ref y);
			x++;
			y++;
			return new Pos(x, y);
		}

		public void SetCurrentPlayer(PlayerColor player)
		{
			SetCurrentPlayer(_handle, player);
		}

		public PlayerColor GetCurrentPlayer()
		{
			return GetCurrentPlayer(_handle);
		}

		public void SetNextPlayer()
		{
			SetNextPlayer(_handle);
		}
	}
}