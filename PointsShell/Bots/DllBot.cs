using System;
using System.Runtime.InteropServices;
using PointsShell.Enums;

namespace PointsShell.Bots
{
	class DllBot : IBot
	{
#if DEBUG
		const string DllName = "../../../../PointsBot/Debug/PointsBot.dll";
#else
		const string DllName = "PointsBot.dll";
#endif

		private IntPtr _handle = IntPtr.Zero;

		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "init")]
		private static extern IntPtr DllInit(int width, int height, IntPtr seed);
		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "final")]
		private static extern void DllFinal(IntPtr field);
		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "put_point")]
		private static extern void DllPutPoint(IntPtr field, int x, int y, PlayerColor player);
		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "remove_last_point")]
		private static extern void DllRemoveLastPoint(IntPtr field);
		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "get_move")]
		private static extern void DllGetMove(IntPtr field, ref int x, ref int y, PlayerColor player);
		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "get_move_with_complexity")]
		private static extern void DllGetMoveWithComplexity(IntPtr field, ref int x, ref int y, PlayerColor player, int complexity);
		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "get_move_with_time")]
		private static extern void DllGetMoveWithTime(IntPtr field, ref int x, ref int y, PlayerColor player, int time);
		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "get_name")]
		private static extern string DllGetName();
		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "get_verion")]
		private static extern string DllGetVersion();

		public DllBot(int width, int height, SurroundCond surCond, BeginPattern beginPattern)
		{
			Init(width, height, surCond, beginPattern);
		}

		~DllBot()
		{
			Final();
		}

		public void Init(int width, int height, SurroundCond surCond, BeginPattern beginPattern)
		{
			if (_handle != IntPtr.Zero)
				Final();
			_handle = DllInit(width, height, new IntPtr(78526081));
		}

		public void Final()
		{
			if (_handle != IntPtr.Zero)
				DllFinal(_handle);
			_handle = IntPtr.Zero;
		}

		public void PutPoint(Pos pos, PlayerColor player)
		{
			if (_handle == IntPtr.Zero)
				throw new Exception("put_point: Not initialized.");
			DllPutPoint(_handle, pos.X, pos.Y, player);
		}

		public void RemoveLastPoint()
		{
			if (_handle == IntPtr.Zero)
				throw new Exception("remove_last_point: Not initialized.");
			DllRemoveLastPoint(_handle);
		}

		public Pos GetMove(PlayerColor player)
		{
			if (_handle == IntPtr.Zero)
				throw new Exception("get_move: Not initialized.");
			var x = 0;
			var y = 0;
			DllGetMove(_handle, ref x, ref y, player);
			return new Pos(x, y);
		}

		public Pos GetMoveWithComplexity(PlayerColor player, int complexity)
		{
			if (_handle == IntPtr.Zero)
				throw new Exception("get_move_with_complexity: Not initialized.");
			var x = 0;
			var y = 0;
			DllGetMoveWithComplexity(_handle, ref x, ref y, player, complexity);
			return new Pos(x, y);
		}

		public Pos GetMoveWithTime(PlayerColor player, int time)
		{
			if (_handle == IntPtr.Zero)
				throw new Exception("get_move_with_time: Not initialized.");
			var x = 0;
			var y = 0;
			DllGetMoveWithTime(_handle, ref x, ref y, player, time);
			return new Pos(x, y);
		}

		public string GetName()
		{
			if (_handle == IntPtr.Zero)
				throw new Exception("get_name: Not initialized.");
			return DllGetName();
		}

		public string GetVersion()
		{
			if (_handle == IntPtr.Zero)
				throw new Exception("get_version: Not initialized.");
			return DllGetVersion();
		}
	}
}